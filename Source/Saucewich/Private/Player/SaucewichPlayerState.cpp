// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "SaucewichGameInstance.h"
#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Player/SaucewichPlayerController.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"
#include "SaucewichLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Log, All)

template <class Fn>
void SafeGameState(ASaucewichPlayerState* const PlayerState, Fn&& Func)
{
	if (const auto GI = PlayerState->GetWorld()->GetGameInstance<USaucewichGameInstance>())
		GI->SafeGameState(Func);
}

void ASaucewichPlayerState::SetWeapon(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
	if (!HasAuthority()) ServerSetWeapon(Slot, Weapon);
	if (CastChecked<AController>(GetOwner())->IsLocalController()) SaveWeaponLoadout();
}

void ASaucewichPlayerState::SaveWeaponLoadout() const
{
	if (const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>())
		GI->SaveWeaponLoadout(WeaponLoadout);
}

void ASaucewichPlayerState::GiveWeapons()
{
	if (const auto Character = GetPawn<ATpsCharacter>())
		for (const auto Weapon : WeaponLoadout)
			Character->GetWeaponComponent()->Give(Weapon);
}

void ASaucewichPlayerState::OnKill()
{
	if (!HasAuthority()) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	++Kill;
	AddScore("Kill");
}

void ASaucewichPlayerState::OnDeath()
{
	if (!HasAuthority()) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	++Death;
}

void ASaucewichPlayerState::AddScore(const FName ScoreID, int32 ActualScore)
{
	if (!HasAuthority()) return;

	const auto GS = GetWorld()->GetGameState<ASaucewichGameState>();
	if (!GS || !GS->CanAddPersonalScore()) return;

	if (ActualScore == 0)
		ActualScore = GS->GetScoreData(ScoreID).Score;
	
	Score += ActualScore;
	MulticastAddScore(ScoreID, ActualScore);

	UE_LOG(LogPlayerState, Log, TEXT("Add %d score to %s by %s"), ActualScore, *GetPlayerName(), *ScoreID.ToString())
}

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	if (Team != NewTeam)
	{
		const auto OldTeam = Team;
		Team = NewTeam;
		OnTeamChanged(OldTeam);
	}
}

void ASaucewichPlayerState::OnTeamChanged(const uint8 OldTeam)
{
	OnTeamChangedDelegate.Broadcast(Team);
	SafeGameState(this, [this, OldTeam](ASaucewichGameState* const GameState)
	{
		GameState->OnPlayerChangedTeam.Broadcast(this, OldTeam, Team);
	});
}

void ASaucewichPlayerState::SetWeapon_Internal(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	if (Slot >= WeaponLoadout.Num()) WeaponLoadout.AddZeroed(Slot - WeaponLoadout.Num() + 1);
	WeaponLoadout[Slot] = Weapon;
}

void ASaucewichPlayerState::ServerSetWeaponLoadout_Implementation(const TArray<TSubclassOf<AWeapon>>& Loadout)
{
	WeaponLoadout = Loadout;
}

bool ASaucewichPlayerState::ServerSetWeaponLoadout_Validate(const TArray<TSubclassOf<AWeapon>>& Loadout)
{
	return true;
}

void ASaucewichPlayerState::LoadWeaponLoadout()
{
	if (const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>())
	{
		for (const auto Wep : GI->GetWeaponLoadout())
			if (Wep) SetWeapon_Internal(Wep.GetDefaultObject()->GetWeaponData().Slot, Wep);
		
		if (!HasAuthority()) ServerSetWeaponLoadout(WeaponLoadout);
	}
}

void ASaucewichPlayerState::MulticastAddScore_Implementation(const FName ScoreName, const int32 ActualScore)
{
	OnScoreAdded.Broadcast(ScoreName, ActualScore);
}

void ASaucewichPlayerState::ServerSetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
}

bool ASaucewichPlayerState::ServerSetWeapon_Validate(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	return true;
}

void ASaucewichPlayerState::SetObjective(const uint8 NewObjective)
{
	if (!HasAuthority()) return;

	if (Objective == NewObjective) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	Objective = NewObjective;
}

void ASaucewichPlayerState::RequestSetPlayerName_Implementation(const FString& NewPlayerName)
{
	if (GetPlayerName() != NewPlayerName && USaucewichLibrary::IsValidPlayerName(NewPlayerName) == ENameValidity::Valid)
	{
		SetPlayerName(NewPlayerName);
	}
}

bool ASaucewichPlayerState::RequestSetPlayerName_Validate(const FString& NewPlayerName)
{
	return true;
}

void ASaucewichPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto PC = Cast<ASaucewichPlayerController>(GetOwner()))
	{
		ASaucewichPlayerController::BroadcastPlayerStateSpawned(PC, this);

		if (PC->IsLocalController())
		{
			LoadWeaponLoadout();
		}
	}
}

void ASaucewichPlayerState::SetPlayerName(const FString& S)
{
	for (const auto Player : TActorRange<APlayerState>{GetWorld()})
	{
		if (Player == this || Player->IsPendingKill()) continue;
		
		auto Name = Player->GetPlayerName();
		if (Name == S)
		{
			auto Cnt = 0;
			for (auto i = Name.Len() - 1; i >= 0 && isdigit(Name[i]); --i, ++Cnt) {}
			const auto Num = FCString::Atoi(*Name + Name.Len() - Cnt) + 1;
			Name.RemoveAt(Name.Len() - Cnt, Cnt, false);
			Name.AppendInt(Num);
			return SetPlayerName(Name);
		}
	}

	auto Old = GetPlayerName();
	Super::SetPlayerName(S);

	if (!Old.IsEmpty())
		if (const auto GameMode = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
			GameMode->OnPlayerChangedName(this, MoveTemp(Old));
}

void ASaucewichPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	OnNameChanged.Broadcast(GetPlayerName());
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
	DOREPLIFETIME(ASaucewichPlayerState, Objective);
	DOREPLIFETIME(ASaucewichPlayerState, Kill);
	DOREPLIFETIME(ASaucewichPlayerState, Death);
	DOREPLIFETIME(ASaucewichPlayerState, WeaponLoadout);
}
