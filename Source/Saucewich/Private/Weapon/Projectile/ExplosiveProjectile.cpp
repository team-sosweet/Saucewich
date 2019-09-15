// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Weapon/Projectile/ExplosiveProjectile.h"

#include "Kismet/GameplayStatics.h"

void AExplosiveProjectile::NotifyHit(UPrimitiveComponent* const MyComp, AActor* const Other, UPrimitiveComponent* const OtherComp,
	const bool bSelfMoved, const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!IsNetMode(NM_Client))
	{
		UE_LOG(LogClass, Log, TEXT(""));
	}
	
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

	Release();
}
