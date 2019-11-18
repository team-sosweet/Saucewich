// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon/Projectile/ExplosiveProjectile.h"

#include "Kismet/GameplayStatics.h"

void AExplosiveProjectile::OnExplode(const FHitResult& Hit)
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
	
	Super::OnExplode(Hit);
}

float AExplosiveProjectile::GetSauceMarkScale() const
{
	return Radius / 75.f;
}
