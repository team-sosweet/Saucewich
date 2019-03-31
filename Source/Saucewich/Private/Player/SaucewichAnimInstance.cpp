// Copyright (c) 2019, 이석진, 강찬. All rights reserved.

#include "SaucewichAnimInstance.h"
#include "GameFramework/Pawn.h"

void USaucewichAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	APawn* Owner = TryGetPawnOwner();
	if (Owner)
	{
		FVector Velocity{ Owner->GetVelocity() };
		MoveDirection = FRotator::NormalizeAxis(Velocity.Rotation().Yaw - Owner->GetActorRotation().Yaw);
		MoveSpeed = Velocity.Size2D();
		AimPitch = Owner->GetBaseAimRotation().Pitch;
	}
}
