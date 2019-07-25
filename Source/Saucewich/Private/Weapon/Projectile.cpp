// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectilePoolComponent.h"
#include "Gun.h"

AProjectile::AProjectile()
	:Movement{CreateDefaultSubobject<UProjectileMovementComponent>("Movement")}
{
}

void AProjectile::Release()
{
	Deactivate();
	Gun->GetProjectilePool()->Release(this);
}

FName AProjectile::GetCollisionProfile() const
{
	return GetStaticMeshComponent()->GetCollisionProfileName();
}

void AProjectile::Activate(const bool bIsCosmetic)
{
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	Movement->SetUpdatedComponent(RootComponent);
	Movement->SetVelocityInLocalSpace(FVector::ForwardVector * Gun->GetProjectileSpeed());
	bCosmetic = bIsCosmetic;
}

void AProjectile::Deactivate()
{
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	Gun = CastChecked<AGun>(GetOwner());
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, const bool bSelfMoved,
	const FVector HitLocation, const FVector HitNormal, const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!bCosmetic)
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
