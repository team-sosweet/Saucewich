// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ProjectilePoolComponent.h"
#include "Engine/World.h"
#include "Projectile.h"

void UProjectilePoolComponent::Release(AProjectile* Projectile)
{
	Pool.Add(Projectile);
}

AProjectile* UProjectilePoolComponent::Spawn(const FQuat& Rotation, const bool bCosmetic)
{
	auto Transform = GetComponentTransform();
	Transform.SetRotation(Rotation);
	return Spawn(Transform, bCosmetic);
}

AProjectile* UProjectilePoolComponent::Spawn(const FTransform& Transform, const bool bCosmetic)
{
	AProjectile* Projectile;

	if (Pool.Num() > 0)
	{
		Projectile = Pool.Pop();
		Projectile->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
	}
	else
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = GetOwner();
		Parameters.Instigator = Parameters.Owner->GetInstigator();
		Projectile = GetWorld()->SpawnActor<AProjectile>(Class, Transform, Parameters);
	}

	Projectile->Activate(bCosmetic);
	return Projectile;
}

void UProjectilePoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	for (auto Projectile : Pool) if (Projectile) Projectile->Destroy();
}
