// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "GunProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AGunProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	void Explode(const FHitResult& Hit) override;
	
	uint8 bCosmetic : 1;

protected:
	float GetSauceMarkScale() const override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	void OnActivated() override;
};
