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
	FName GetCollisionProfile() const;

	void Release();
	void Activate(bool bIsCosmetic);
	void Deactivate();

	uint8 bCosmetic : 1;

protected:
	void BeginPlay() override;
	void LifeSpanExpired() override { Release(); }
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	class AGun* Gun;
};
