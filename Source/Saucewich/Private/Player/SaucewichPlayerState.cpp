// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "SaucewichGameInstance.h"
#include "SaucewichGameMode.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichPlayerState, Log, All)

template <class Fn>
void SafeGameState(ASaucewichPlayerState* const PlayerState, Fn&& Func)
{
	if (const auto GI = PlayerState->GetWorld()->GetGameInstance<USaucewichGameInstance>())
		GI->SafeGameState(Func);
	else UE_LOG(LogSaucewichPlayerState, Error, TEXT("Failed to cast game instance to SaucewichGameInstance"));
}

void ASaucewichPlayerState::SetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	MulticastSetWeapon(Slot, Weapon);
}

bool ASaucewichPlayerState::SetWeapon_Validate(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	return true;
}

void ASaucewichPlayerState::MulticastSetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	if (Weapons.Num() <= Slot) Weapons.AddZeroed(Slot - Weapons.Num() + 1);
	Weapons[Slot] = Weapon;
	SafeGameState(this, [this, Slot, Weapon](ASaucewichGameState* const GameState)
	{
		GameState->OnPlayerChangedWeapon.Broadcast(this, Slot, Weapon);
	});
}

void ASaucewichPlayerState::GiveWeapons()
{
	if (const auto Character = GetPawn<ATpsCharacter>())
		for (const auto Weapon : Weapons)
			Character->GetWeaponComponent()->Give(Weapon);
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

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
}
