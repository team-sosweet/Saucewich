// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Projectile.generated.h"

UCLASS()
class AProjectile : public AStaticMeshActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UProjectileMovementComponent* Movement;

public:
	AProjectile();

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float Speed) const;
	FName GetCollisionProfile() const;

	void Release();
	virtual void SetActivated(bool bActive);
	void SetPool(class UProjectilePoolComponent* NewPool) { Pool = NewPool; }

protected:
	void LifeSpanExpired() override { Release(); }
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UProjectilePoolComponent* Pool;
};
