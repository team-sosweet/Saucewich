// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GunProjectile.h"
#include "GameFramework/Pawn.h"
#include "Gun.h"

void AGunProjectile::OnActivated()
{
	Super::OnActivated();
	Gun = CastChecked<AGun>(GetOwner());

	if (const auto Data = Gun->GetData<FGunData>(FILE_LINE_FUNC))
	{
		SetSpeed(Data->ProjectileSpeed);
	}
}

void AGunProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	const auto Data = Gun->GetData<FGunData>(FILE_LINE_FUNC);
	if (Data && !bCosmetic && Gun)
	{
		const auto Damage = Data->Damage;
		Other->TakeDamage(
			Damage,
			FPointDamageEvent{Damage, Hit, GetVelocity().GetSafeNormal(), Data->DamageType},
			GetInstigator()->GetController(),
			GetOwner()
		);
	}

	Release();
}
