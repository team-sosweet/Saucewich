// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SauceProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ASauceProjectile::ASauceProjectile()
	:Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") },
	Movement{ CreateDefaultSubobject<UProjectileMovementComponent>("Movement") }
{
	RootComponent = Mesh;
}

void ASauceProjectile::SetColor(const FLinearColor& Color)
{
	ColorDynamicMaterial->SetVectorParameterValue("Base Color", Color);
}

void ASauceProjectile::SetSpeed(float Speed)
{
	Movement->Velocity = GetActorForwardVector() * Speed + GetInstigator()->GetVelocity() * CharacterVelocityApplyRate;
}

void ASauceProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (Other)
	{
		const auto Instigtor{ GetInstigator() };
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

	static const FName SlotName{ "Color" };
	ColorDynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(Mesh->GetMaterialIndex(SlotName)), this);
	Mesh->SetMaterialByName(SlotName, ColorDynamicMaterial);
}

void ASauceProjectile::BeginReuse()
{
	Movement->SetUpdatedComponent(RootComponent);
}
