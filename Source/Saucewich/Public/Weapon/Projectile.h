// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PoolActor.h"
#include "Projectile.generated.h"

UCLASS()
class AProjectile : public APoolActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UProjectileMovementComponent* Movement;


public:
	AProjectile();
	void SetSpeed(float Speed) const;
	FName GetCollisionProfile() const;

protected:
	void OnActivated() override;
	void OnReleased() override;
};
