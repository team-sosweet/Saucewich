// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "TpsCharacterMovementComponent.generated.h"

UCLASS()
class SAUCEWICH_API UTpsCharacterMovementComponent final : public UCharacterMovementComponent
{
	GENERATED_BODY()

	float GetMaxSpeed() const override;
};
