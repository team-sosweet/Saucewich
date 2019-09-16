// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon/Projectile/ExplosiveProjectile.h"

#include "Kismet/GameplayStatics.h"

void AExplosiveProjectile::ApplyDamage()
{
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		GetActorLocation(),
		Radius,
		DamageType,
		{},
		this,
		GetInstigatorController(),
		false,
		DamagePreventionChannel
	);
}
