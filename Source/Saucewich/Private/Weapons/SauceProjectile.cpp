// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SauceProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASauceProjectile::ASauceProjectile()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") },
	Movement{ CreateDefaultSubobject<UProjectileMovementComponent>("Movement") }
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = Mesh;
}

void ASauceProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASauceProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}
