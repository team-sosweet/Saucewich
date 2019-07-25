// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ProjectilePoolComponent.h"
#include "Engine/World.h"
#include "Projectile.h"

AProjectile* UProjectilePoolComponent::Spawn(const FRotator& Rotation)
{
	return Spawn(Rotation.Quaternion());
}

AProjectile* UProjectilePoolComponent::Spawn(const FQuat& Rotation)
{
	auto Transform = GetComponentTransform();
	Transform.SetRotation(Rotation);
	return Spawn(Transform);
}

AProjectile* UProjectilePoolComponent::Spawn(const FTransform& Transform)
{
	if (Pool.Num() > 0)
	{
		const auto Projectile = Pool.Pop();
		Projectile->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
		Projectile->SetActivated(true);
		return Projectile;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = GetOwner();
	Parameters.Instigator = Parameters.Owner->GetInstigator();

	const auto Projectile = GetWorld()->SpawnActor<AProjectile>(Class, Transform, Parameters);
	Projectile->SetPool(this);
	return Projectile;
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
