// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SauceProjectile.generated.h"

UCLASS()
class SAUCEWICH_API ASauceProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASauceProjectile();
	
	void SetUsing(bool bUse);
	bool IsUsing() const { return bUsing; }

private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	uint8 bUsing : 1;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
