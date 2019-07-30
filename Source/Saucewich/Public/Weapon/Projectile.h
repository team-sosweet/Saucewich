// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "PoolActor.h"
#include "Colorable.h"
#include "Projectile.generated.h"

UCLASS()
class AProjectile : public APoolActor, public IColorable
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UProjectileMovementComponent* Movement;

public:
	AProjectile();
	void ResetSpeed() const;
	void SetSpeed(float Speed) const;
	FName GetCollisionProfile() const;

	void SetColor(const FLinearColor& NewColor) override;

protected:
	void PostInitializeComponents() override;
	void OnActivated() override;
	void OnReleased() override;

private:
	UMaterialInstanceDynamic* Material;
};
