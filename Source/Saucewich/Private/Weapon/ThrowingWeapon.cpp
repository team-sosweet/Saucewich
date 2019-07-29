// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ThrowingWeapon.h"
#include "Engine/World.h"
#include "ActorPool.h"
#include "Projectile.h"
#include "SaucewichGameInstance.h"

void AThrowingWeapon::SlotP()
{
	if (bReloading) return;

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = GetInstigator();

	if (const auto Thrown = ProjectilePool->Spawn<AProjectile>(ProjectileClass, ThrowOffset * GetActorTransform(), Parameters))
	{
		Thrown->ResetSpeed();
		bReloading = true;
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
			ReloadingTime = 0.f;
			bReloading = false;
		}
	}
}
