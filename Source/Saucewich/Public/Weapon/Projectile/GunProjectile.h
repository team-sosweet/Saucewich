// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "GunProjectile.generated.h"

UCLASS(Abstract)
class SAUCEWICH_API AGunProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	uint8 bCosmetic : 1;

private:
	void OnActivated() override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};
