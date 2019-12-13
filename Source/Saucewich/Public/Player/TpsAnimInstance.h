// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "TpsAnimInstance.generated.h"

UCLASS()
class SAUCEWICH_API UTpsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitchMul = 1.f;
};
