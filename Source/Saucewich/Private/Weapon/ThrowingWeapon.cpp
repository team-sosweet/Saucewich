// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ThrowingWeapon.h"
#include "Engine/World.h"
#include "ActorPool.h"
#include "SaucewichGameInstance.h"

void AThrowingWeapon::SlotP()
{
	if (bReloading) return;

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = GetInstigator();
	//if (const auto Thrown = ProjectilePool->Spawn<AProjectile>(ProjectileClass, GetActorTransform(), Parameters))
	{
		
	}
}

void AThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();
	ProjectilePool = CastChecked<USaucewichGameInstance>(GetGameInstance())->GetActorPool();
}

void AThrowingWeapon::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bReloading)
	{
		ReloadingTime += DeltaSeconds;
		if (ReloadingTime >= ReloadTime)
		{
			bReloading = false;
			ReloadingTime = 0.f;
		}
	}
}
