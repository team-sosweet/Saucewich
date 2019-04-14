// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PoolActor.h"
#include "SauceProjectile.generated.h"

UCLASS()
class SAUCEWICH_API ASauceProjectile : public APoolActor
{
	GENERATED_BODY()
	
public:	
	ASauceProjectile();
	void Init(float Damage, float Speed);

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	UPROPERTY(EditDefaultsOnly)
	float CharacterVelocityApplyRate = .5f;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginReuse() override;

	float Damage;
};
