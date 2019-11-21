// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "TpsCharacterMovementComponent.generated.h"

UCLASS()
class SAUCEWICH_API UTpsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	float GetMaxSpeed() const override;
};
