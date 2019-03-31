// Copyright (c) 2019, 이석진, 강찬. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SaucewichAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SAUCEWICH_API USaucewichAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	float MoveDirection;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	float MoveSpeed;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	float AimPitch;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	float AimYaw;

	UPROPERTY(EditDefaultsOnly)
	float AimYawInterpSpeed = 10.f;

	float AimYawOffset;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	float HeadYaw;

	UPROPERTY(EditDefaultsOnly)
	float MinHeadYaw = -45.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHeadYaw = 120.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Transient, meta = (AllowPrivateAccess = true))
	uint8 bFalling : 1;

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
