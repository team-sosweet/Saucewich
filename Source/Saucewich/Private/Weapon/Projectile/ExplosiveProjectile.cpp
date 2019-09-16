// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon/Projectile/ExplosiveProjectile.h"

#include "Kismet/GameplayStatics.h"

void AExplosiveProjectile::ApplyDamage() const
{
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		GetActorLocation(),
		Radius,
		DamageType,
		{},
		GetOwner(),
		GetInstigatorController(),
		false,
		DamagePreventionChannel
	);
}
