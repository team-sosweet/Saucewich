// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "SaucewichGameMode.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"

void ASaucewichPlayerState::SetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	MulticastSetWeapon(Slot, Weapon);
}

bool ASaucewichPlayerState::SetWeapon_Validate(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	return Weapon && Slot < Weapons.Num();
}

void ASaucewichPlayerState::MulticastSetWeapon_Implementation(const uint8 Slot, const TSubclassOf<AWeapon> Weapon)
{
	Weapons[Slot] = Weapon;
	if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		GS->OnPlayerChangedWeapon.Broadcast(this, Slot, Weapon);
	}
}

void ASaucewichPlayerState::GiveWeapons()
{
	if (const auto Character = GetPawn<ATpsCharacter>())
	{
		for (auto i = 0; i < Weapons.Num(); ++i)
		{
			if (const auto Weapon = Weapons[i] ? Weapons[i] : i < DefaultWeapons.Num() ? DefaultWeapons[i] : nullptr)
			{
				Character->GetWeaponComponent()->Give(Weapon);
			}
		}
	}
}

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	const auto OldTeam = Team;
	Team = NewTeam;
	OnTeamChanged(OldTeam);
}

void ASaucewichPlayerState::OnTeamChanged(const uint8 OldTeam)
{
	OnTeamChangedDelegate.Broadcast(Team);
	if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		GS->OnPlayerChangedTeam.Broadcast(this, OldTeam, Team);
	}
}

void ASaucewichPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (const auto Gm = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
	{
		Weapons.Init(
			nullptr,
			GetDefault<ATpsCharacter>(
				Gm->GetDefaultPawnClassForController(static_cast<AController*>(GetOwner()))
			)->GetWeaponComponent()->GetSlots()
		);
	}
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
}
