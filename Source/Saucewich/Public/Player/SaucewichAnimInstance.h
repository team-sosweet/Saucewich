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

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
