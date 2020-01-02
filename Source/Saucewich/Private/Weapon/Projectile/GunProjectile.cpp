// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Weapon/Projectile/GunProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Weapon/Gun.h"

void AGunProjectile::OnExplode(const FHitResult& Hit)
{
	if (const auto Other = Hit.GetActor())
	{
		auto&& Data = CastChecked<AGun>(GetOwner())->GetGunData();
		const auto Damage = Data.Damage;
		Other->TakeDamage(
			Damage,
			FPointDamageEvent{Damage, Hit, GetVelocity().GetSafeNormal(), Data.DamageType.LoadSynchronous()},
			GetInstigatorController(),
			GetOwner()
		);
	}
	
	Super::OnExplode(Hit);
}

float AGunProjectile::GetSauceMarkScale() const
{
	auto&& S = GetMesh()->GetRelativeScale3D();
	return (S.X + S.Y + S.Z) / 6.f;
}

void AGunProjectile::OnActivated()
{
	Super::OnActivated();

	const auto Gun = Cast<AGun>(GetOwner());
	check(Gun);
	SetSpeed(Gun->GetGunData().ProjectileSpeed);
}

void AGunProjectile::NotifyHit(UPrimitiveComponent* const MyComp, AActor* const Other, UPrimitiveComponent* const OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	Explode(Hit);
}
