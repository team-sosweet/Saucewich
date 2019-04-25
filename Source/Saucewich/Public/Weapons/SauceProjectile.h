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

	UFUNCTION(BlueprintCallable)
	void SetColor(const FLinearColor& Color);

	void SetSpeed(float Speed);

	float Damage;

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	UPROPERTY(EditDefaultsOnly)
	float CharacterVelocityApplyRate{ .5f };

	class UMaterialInstanceDynamic* ColorDynamicMaterial;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;
	virtual void BeginReuse() override;
};
