// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ProjectilePoolComponent.h"
#include "Engine/World.h"
#include "Projectile.h"

AProjectile* UProjectilePoolComponent::Spawn()
{
	if (Pool.Num() > 0)
	{
		const auto Projectile = Pool.Pop();
		Projectile->SetActorLocation(GetComponentLocation());
		Projectile->SetActorRotation(GetComponentRotation());
		Projectile->SetActivated(true);
		return Projectile;
	}

	FActorSpawnParameters Parameters;
	Parameters.Owner = GetOwner();
	Parameters.Instigator = Parameters.Owner->GetInstigator();

	auto SpawnTransform = GetComponentTransform();
	SpawnTransform.SetScale3D(GetDefault<AProjectile>(Class)->GetActorScale());

	const auto Projectile = GetWorld()->SpawnActor<AProjectile>(Class, SpawnTransform, Parameters);
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
