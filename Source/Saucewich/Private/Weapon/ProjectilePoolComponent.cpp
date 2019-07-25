// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ProjectilePoolComponent.h"
#include "Engine/World.h"
#include "Projectile.h"

AProjectile* UProjectilePoolComponent::Spawn(const FQuat& Rotation, const bool bCosmetic)
{
	auto Transform = GetComponentTransform();
	Transform.SetRotation(Rotation);
	return Spawn(Transform, bCosmetic);
}

AProjectile* UProjectilePoolComponent::Spawn(const FTransform& Transform, const bool bCosmetic)
{
	if (Pool.Num() > 0)
	{
		const auto Projectile = Pool.Pop();
		Projectile->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
		Projectile->Activate(bCosmetic);
		return Projectile;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = GetOwner();
	Parameters.Instigator = Parameters.Owner->GetInstigator();

	return GetWorld()->SpawnActor<AProjectile>(Class, Transform, Parameters);
}

void UProjectilePoolComponent::Release(AProjectile* Projectile)
{
	Pool.Add(Projectile);
}

void UProjectilePoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (auto Projectile : Pool)
	{
		Projectile->Destroy();
	}
}
