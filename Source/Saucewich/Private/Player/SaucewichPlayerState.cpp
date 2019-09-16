// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "SaucewichGameInstance.h"
#include "Online/SaucewichGameMode.h"
#include "Online/SaucewichGameState.h"
#include "Player/TpsCharacter.h"
#include "Weapon/Weapon.h"
#include "Weapon/WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichPlayerState, Log, All)

template <class Fn>
void SafeGameState(ASaucewichPlayerState* const PlayerState, Fn&& Func)
{
	if (const auto GI = PlayerState->GetWorld()->GetGameInstance<USaucewichGameInstance>())
		GI->SafeGameState(Func);
	else UE_LOG(LogSaucewichPlayerState, Error, TEXT("Failed to cast game instance to SaucewichGameInstance"));
}

void ASaucewichPlayerState::SetWeapon(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	SetWeapon_Internal(Slot, Weapon);
	if (!HasAuthority()) ServerSetWeapon(Slot, Weapon);
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
	if (HasAuthority())
	{
		++Kill;
		AddScore("Kill");
	}
}

void ASaucewichPlayerState::OnDeath()
{
	if (HasAuthority())
	{
		++Death;
	}
}

void ASaucewichPlayerState::AddScore(const FName ScoreID, int32 ActualScore)
{
	if (!HasAuthority()) return;

	if (ActualScore == 0)
		if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
			ActualScore = GS->GetScoreData(ScoreID).Score;
	
	Score += ActualScore;
	MulticastAddScore(ScoreID, ActualScore);
}

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	if (Team == NewTeam) return;
	const auto OldTeam = Team;
	Team = NewTeam;
	OnTeamChanged(OldTeam);
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

void ASaucewichPlayerState::LoadWeaponLoadout(ATpsCharacter* const Char)
{
	if (Char->IsLocallyControlled())
	{
		if (const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>())
		{
			auto& Loadout = GI->GetWeaponLoadout();
			for (auto i = 0; i < Loadout.Num(); ++i)
			{
				SetWeapon(i, Loadout[i]);
			}
		}
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

void ASaucewichPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PC = Cast<ASaucewichPlayerController>(GetOwner()))
	{
		ASaucewichPlayerController::BroadcastPlayerStateSpawned(PC, this);

		FOnCharacterSpawnedSingle OnCharacterSpawned;
		OnCharacterSpawned.BindDynamic(this, &ASaucewichPlayerState::LoadWeaponLoadout);
		PC->SafeCharacter(OnCharacterSpawned);
	}
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
