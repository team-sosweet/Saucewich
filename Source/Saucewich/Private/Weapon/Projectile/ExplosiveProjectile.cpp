// Copyright 2019 Othereum. All Rights Reserved.

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
	
	UGameplayStatics::PlayWorldCameraShake(this, CameraShake.LoadSynchronous(), Hit.Location, 0.f, Radius * 2.5f);

	Super::OnExplode(Hit);
}

float AExplosiveProjectile::GetSauceMarkScale() const
{
	return Radius / 75.f;
}
