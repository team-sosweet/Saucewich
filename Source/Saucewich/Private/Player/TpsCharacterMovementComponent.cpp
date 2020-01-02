// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Player/TpsCharacterMovementComponent.h"
#include "Player/TpsCharacter.h"

float UTpsCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * CastChecked<ATpsCharacter>(GetOwner())->GetSpeedRatio();
}
