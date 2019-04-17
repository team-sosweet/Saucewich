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

void ASauceProjectile::Init(float NewDamage, float NewSpeed)
{
	Damage = NewDamage;
	Movement->Velocity = GetActorForwardVector() * NewSpeed + GetInstigator()->GetVelocity() * CharacterVelocityApplyRate;
}

void ASauceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (Other)
	{
		APawn* const Instigtor = GetInstigator();
		if (Instigator)
		{
			static const FDamageEvent DamageEvent;
			Other->TakeDamage(Damage, DamageEvent, Instigator->GetController(), this);
		}
	}
	ReturnToPool();
}

void ASauceProjectile::BeginPlay()
{
	Super::BeginPlay();
	Mesh->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ASauceProjectile::BeginReuse()
{
	Movement->SetUpdatedComponent(RootComponent);
}
