// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "PoolActor.h"
#include "Colorable.h"
#include "Projectile.generated.h"

/*
 * 발사체는 pooling 가능하여 대량으로 생산할 수 있으며, ProjectileMovementComponent를 가진 클래스입니다.
 */
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
