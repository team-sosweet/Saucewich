// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Weapon/Projectile/Projectile.h"
#include "ExplosiveProjectile.generated.h"

UCLASS()
class SAUCEWICH_API AExplosiveProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	float GetSauceMarkScale() const override;
	void OnExplode(const FHitResult& Hit) override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Damage = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float Radius = 300;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> DamagePreventionChannel;
};
