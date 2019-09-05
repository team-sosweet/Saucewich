// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/WeaponSharedData.h"
#include "Engine/CollisionProfile.h"
#include "GunSharedData.generated.h"

UCLASS()
class SAUCEWICH_API UGunSharedData : public UWeaponSharedData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FCollisionProfileName PawnOnly;

	UPROPERTY(EditAnywhere)
	FCollisionProfileName NoPawn;

	UPROPERTY(EditAnywhere)
	float TraceStartOffset = 10;
};
