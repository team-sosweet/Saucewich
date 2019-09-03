// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ThrowingWeapon.h"

#include "Engine/World.h"

#include "Saucewich.h"
#include "Entity/ActorPool.h"
#include "Weapon/Projectile/Projectile.h"

void AThrowingWeapon::SlotP()
{
	if (bReloading) return;

	const auto Data = GetData<FThrowingWeaponData>(FILE_LINE_FUNC);
	if (!Data) return;

	FActorSpawnParameters Parameters;
	Parameters.Owner = this;
	Parameters.Instigator = GetInstigator();

	if (const auto Thrown = GetPool()->Spawn<AProjectile>(Data->ProjectileClass, Data->ThrowOffset * GetActorTransform(), Parameters))
	{
		Thrown->ResetSpeed();
		Thrown->SetColor(GetColor());
		bReloading = true;
	}
}

void AThrowingWeapon::OnActivated()
{
	Super::OnActivated();
	
	ReloadingTime = 0;
	bReloading = false;

	if (GetData<FThrowingWeaponData>(FILE_LINE_FUNC))
		SetActorTickEnabled(false);
}

void AThrowingWeapon::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bReloading)
	{
		ReloadingTime += DeltaSeconds;
		if (ReloadingTime >= static_cast<const FThrowingWeaponData*>(GetData(FILE_LINE_FUNC))->ReloadTime)
		{
			ReloadingTime = 0;
			bReloading = false;
		}
	}
}
