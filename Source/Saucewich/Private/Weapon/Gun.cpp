// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Gun.h"
#include "UnrealNetwork.h"
#include "TpsCharacter.h"
#include "WeaponComponent.h"

void AGun::FireP()
{
	bFiring = true;
	FireLag = 0.f;
	if (LastFire + 60.f / Rpm <= GetGameTimeSinceCreation())
	{
		Shoot();
		LastFire = GetGameTimeSinceCreation();
	}
}

void AGun::FireR()
{
	bFiring = false;
}

void AGun::SlotP()
{
	GetCharacter()->GetWeaponComponent()->TrySelectWeapon(GetSlot());
}

void AGun::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFiring)
	{
		const auto CurTime = GetGameTimeSinceCreation();
		const auto Delay = 60.f / Rpm;
		for (; FireLag >= Delay; FireLag -= Delay)
		{
			Shoot();
			LastFire = CurTime;
		}
		FireLag += DeltaSeconds;
	}
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, Clip);
	DOREPLIFETIME(AGun, bFiring);
}
