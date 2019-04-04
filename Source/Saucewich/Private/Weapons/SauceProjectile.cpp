// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SauceProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASauceProjectile::ASauceProjectile()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") },
	Movement{ CreateDefaultSubobject<UProjectileMovementComponent>("Movement") },
	bUsing{ true }
{
	RootComponent = Mesh;
}

void ASauceProjectile::SetUsing(bool bUse)
{
	if (bUsing != bUse)
	{
		Mesh->SetCollisionEnabled(bUse ? GetClass()->GetDefaultObject<ASauceProjectile>()->Mesh->GetCollisionEnabled() : ECollisionEnabled::NoCollision);
		Mesh->SetVisibility(bUse);

		if (bUse)
		{
			Movement->Velocity = GetActorForwardVector() * Movement->InitialSpeed;
			Movement->SetUpdatedComponent(RootComponent);
		}

		bUsing = bUse;
	}
}

void ASauceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	SetUsing(false);
}
