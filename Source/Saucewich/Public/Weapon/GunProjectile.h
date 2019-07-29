// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "GunProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AGunProjectile final : public AProjectile
{
	GENERATED_BODY()

public:
	uint8 bCosmetic : 1;

private:
	void OnActivated() override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	class AGun* Gun;
};
