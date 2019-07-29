// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "TpsAnimInstance.generated.h"

UCLASS()
class UTpsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitchOffsetWhileNoTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;
};
