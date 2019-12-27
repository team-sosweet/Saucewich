// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/SaucewichPlayerState.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Player/SaucewichPlayerController.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"
#include "Saucewich.h"
#include "SaucewichInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Log, All)

void ASaucewichPlayerState::SetWeapon(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
	if (!HasAuthority()) ServerSetWeapon(Slot, Weapon);
	if (CastChecked<AController>(GetOwner())->IsLocalController()) SaveWeaponLoadout();
}

void ASaucewichPlayerState::SaveWeaponLoadout()
{
	SaveConfig();
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

	const auto GS = CastChecked<AGameState>(GetWorld()->GetGameState());
	if (!GS->IsMatchInProgress()) return;

	++Kill;
	AddScore(TEXT("Kill"));
}

void ASaucewichPlayerState::OnDeath()
{
	if (!HasAuthority()) return;

	const auto GS = GetWorld()->GetGameState<AGameState>();
	if (!GS || !GS->IsMatchInProgress()) return;

	++Death;
}

void ASaucewichPlayerState::AddScore(const FName ScoreID, int32 ActualScore, const bool bForce)
{
	if (!HasAuthority()) return;

	if (!bForce)
	{
		const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
		if (!GS->CanAddPersonalScore()) return;
	}

	const auto GI = GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
	if (ActualScore == 0)
		ActualScore = GI->GetScoreData(ScoreID).Score;
	
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
	OnTeamChangedNative.Broadcast(Team);

	if (const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState(), ECastCheckedType::NullAllowed))
		GS->OnPlayerChangedTeam.Broadcast(this, OldTeam, Team);
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

void ASaucewichPlayerState::BindOnTeamChanged(FOnTeamChangedNative::FDelegate&& Callback)
{
	Callback.Execute(Team);
	OnTeamChangedNative.Add(MoveTemp(Callback));
}

void ASaucewichPlayerState::RequestSetPlayerName_Implementation(const FString& NewPlayerName)
{
	if (GetPlayerName() != NewPlayerName && USaucewich::IsValidPlayerName(NewPlayerName) == ENameValidity::Valid)
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
	}
}

void ASaucewichPlayerState::SetPlayerName(const FString& S)
{
	for (const auto Player : TActorRange<APlayerState>{GetWorld()})
	{
		if (Player == this || Player->IsPendingKill()) continue;
		
		auto Name = Player->GetPlayerName();
		if (Name.Equals(S, ESearchCase::IgnoreCase))
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
