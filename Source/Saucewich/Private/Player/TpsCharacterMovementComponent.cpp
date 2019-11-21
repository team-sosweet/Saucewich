// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "TpsCharacterMovementComponent.h"
#include "TpsCharacter.h"

float UTpsCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * CastChecked<ATpsCharacter>(GetOwner())->GetSpeedRatio();
}
