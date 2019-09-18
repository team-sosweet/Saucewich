// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AProjectile::AProjectile()
	:Mesh{CreateDefaultSubobject<UStaticMeshComponent>("Mesh")},
	Movement{CreateDefaultSubobject<UProjectileMovementComponent>("Movement")}
{
	RootComponent = Mesh;
	InitialLifeSpan = 5;
}

void AProjectile::ResetSpeed() const
{
	SetSpeed(Movement->InitialSpeed);
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

FLinearColor AProjectile::GetColor() const
{
	FLinearColor Color;
	if (Material) Material->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

void AProjectile::SetColor(const FLinearColor& NewColor)
{
	if (Material) Material->SetVectorParameterValue("Color", NewColor);
}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Material = Mesh->CreateDynamicMaterialInstance(FMath::Max(Mesh->GetMaterialIndex("TeamColor"), 0));
}
