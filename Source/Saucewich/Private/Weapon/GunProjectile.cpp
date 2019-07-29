// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GunProjectile.h"
#include "GameFramework/Pawn.h"
#include "Gun.h"

void AGunProjectile::OnActivated()
{
	Super::OnActivated();
	Gun = CastChecked<AGun>(GetOwner());
	SetSpeed(Gun->GetProjectileSpeed());
}

void AGunProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!bCosmetic && Gun)
	{
		const auto Damage = Gun->GetDamage();
		Other->TakeDamage(
			Damage,
			FPointDamageEvent{Damage, Hit, GetVelocity().GetSafeNormal(), Gun->GetDamageType()},
			GetInstigator()->GetController(),
			GetOwner()
		);
	}

	Release();
}
