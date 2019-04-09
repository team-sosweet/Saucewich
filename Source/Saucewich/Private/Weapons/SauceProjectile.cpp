// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SauceProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASauceProjectile::ASauceProjectile()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") },
	Movement{ CreateDefaultSubobject<UProjectileMovementComponent>("Movement") }
{
	RootComponent = Mesh;
}

void ASauceProjectile::BeginPlay()
{
	Super::BeginPlay();

	Movement->Velocity += GetInstigator()->GetVelocity() * CharacterVelocityApplyRate;
}

void ASauceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	ReturnToPool();
}

void ASauceProjectile::BeginReuse()
{
	Movement->SetUpdatedComponent(RootComponent);
	Movement->Velocity = GetActorForwardVector() * Movement->InitialSpeed + GetInstigator()->GetVelocity() * CharacterVelocityApplyRate;
}
