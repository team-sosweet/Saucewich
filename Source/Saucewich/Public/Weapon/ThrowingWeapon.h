// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ThrowingWeapon.generated.h"

UCLASS(Abstract)
class AThrowingWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	void SlotP() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<class AProjectile> ProjectileClass;
};
