// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "ExplosiveProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AExplosiveProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	void Explode(const FHitResult& Hit) override;
	
protected:
	float GetSauceMarkScale() const override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Damage = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Radius = 300;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> DamagePreventionChannel;
};
