// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "WeaponEnum.h"
#include "SaucewichAnimInstance.generated.h"

UCLASS()
class SAUCEWICH_API USaucewichAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	float MoveDirection;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	float MoveSpeed;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Transient)
	FRotator AimOffset;
	FRotator AimOffsetOffset;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	float HeadYaw;

	UPROPERTY(EditDefaultsOnly)
	float MinHeadYaw = -45.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHeadYaw = 120.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	uint8 bFalling : 1;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient)
	EWeaponGripType WeaponGripType;

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
