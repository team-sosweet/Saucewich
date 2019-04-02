// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SaucewichAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

void USaucewichAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	APawn* Owner = TryGetPawnOwner();
	if (Owner)
	{
		FVector Velocity{ Owner->GetVelocity() };
		FRotator OwnerRot = Owner->GetActorRotation();
		MoveDirection = FRotator::NormalizeAxis(Velocity.Rotation().Yaw - OwnerRot.Yaw);
		MoveSpeed = Velocity.Size2D();

		FRotator NewAimOffset = Owner->GetBaseAimRotation() - OwnerRot;
		NewAimOffset.Normalize();
		AimOffset = NewAimOffset;

		bFalling = Owner->GetMovementComponent()->IsFalling();
	}
}
