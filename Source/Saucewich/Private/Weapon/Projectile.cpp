// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Gun.h"

AProjectile::AProjectile()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
	Movement{CreateDefaultSubobject<UProjectileMovementComponent>("Movement")}
{
	RootComponent = Mesh;
}

void AProjectile::OnActivated()
{
	Gun = CastChecked<AGun>(GetOwner());
	Movement->SetUpdatedComponent(Mesh);
	Movement->SetVelocityInLocalSpace(FVector::ForwardVector * Gun->GetProjectileSpeed());
}

FName AProjectile::GetCollisionProfile() const
{
	return Mesh->GetCollisionProfileName();
}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, const bool bSelfMoved,
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
