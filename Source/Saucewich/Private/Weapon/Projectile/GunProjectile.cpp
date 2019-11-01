// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GunProjectile.h"
#include "GameFramework/Pawn.h"
#include "Gun.h"

void AGunProjectile::OnActivated()
{
	Super::OnActivated();

	const auto Gun = Cast<AGun>(GetOwner());
	if (!ensure(Gun)) return;
	
	SetSpeed(Gun->GetGunData().ProjectileSpeed);
}

void AGunProjectile::NotifyHit(UPrimitiveComponent* const MyComp, AActor* const Other, UPrimitiveComponent* const OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!bCosmetic && Other)
	{
		if (const auto Gun = Cast<AGun>(GetOwner()))
		{
			auto& Data = Gun->GetGunData();
			const auto Damage = Data.Damage;
			Other->TakeDamage(
				Damage,
				FPointDamageEvent{Damage, Hit, GetVelocity().GetSafeNormal(), Data.DamageType},
				Instigator ? Instigator->GetController() : nullptr,
				GetOwner()
			);
		}
	}

	Release();
}
