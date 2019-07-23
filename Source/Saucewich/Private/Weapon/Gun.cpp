// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"
#include "UnrealNetwork.h"
#include "TpsCharacter.h"
#include "WeaponComponent.h"

void AGun::FireP()
{
	bFiring = true;
}

void AGun::FireR()
{
	bFiring = false;
}

void AGun::SlotP()
{
	GetCharacter()->GetWeaponComponent()->TrySelectWeapon(GetSlot());
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
	DOREPLIFETIME(AGun, bFiring);
}
