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

void AProjectile::SetSpeed(const float Speed) const
{
	Movement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
}

void AProjectile::OnActivated()
{
	Movement->SetUpdatedComponent(Mesh);
}

void AProjectile::OnReleased()
{
	Movement->SetUpdatedComponent(nullptr);
}

FName AProjectile::GetCollisionProfile() const
{
	return Mesh->GetCollisionProfileName();
}
