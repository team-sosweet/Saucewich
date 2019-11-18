// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon/Projectile/ExplosiveProjectile.h"

#include "Kismet/GameplayStatics.h"

void AExplosiveProjectile::Explode(const FHitResult& Hit)
{
	Super::Explode(Hit);
	
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		Hit.Location,
		Radius,
		DamageType,
		{},
		GetOwner(),
		GetInstigatorController(),
		false,
		DamagePreventionChannel
	);
}

float AExplosiveProjectile::GetSauceMarkScale() const
{
	return Radius / 75.f;
}
