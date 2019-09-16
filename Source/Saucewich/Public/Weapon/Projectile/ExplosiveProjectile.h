// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "ExplosiveProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AExplosiveProjectile final : public AProjectile
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyDamage() const;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere)
	float Damage = 100;

	UPROPERTY(EditAnywhere)
	float Radius = 300;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> DamagePreventionChannel;
};
