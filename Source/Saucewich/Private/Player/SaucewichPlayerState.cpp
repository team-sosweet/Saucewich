// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichPlayerState.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "SaucewichGameMode.h"
#include "SaucewichGameState.h"
#include "TpsCharacter.h"
#include "Weapon.h"
#include "WeaponComponent.h"

void ASaucewichPlayerState::SetTeam(const uint8 NewTeam)
{
	Team = NewTeam;
	OnTeamChanged();
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

void ASaucewichPlayerState::SetWeapon_Implementation(const uint8 Slot, UClass* const Weapon)
{
	Weapons[Slot] = Weapon;
	OnRep_Weapons();
}

bool ASaucewichPlayerState::SetWeapon_Validate(const uint8 Slot, UClass* const Weapon)
{
	return Weapon && Slot < Weapons.Num();
}

void ASaucewichPlayerState::OnTeamChanged() const
{
	OnTeamChangedDelegate.Broadcast(Team);
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

void ASaucewichPlayerState::OnRep_Weapons()
{
	if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		GS->OnWeaponChanged.Broadcast(this);
	}
}

void ASaucewichPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaucewichPlayerState, Team);
	DOREPLIFETIME(ASaucewichPlayerState, Weapons);
}
