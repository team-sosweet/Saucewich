// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "GunProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AGunProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	float GetSauceMarkScale() const override;
	void OnExplode(const FHitResult& Hit) override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	void OnActivated() override;
};
