// Copyright (c) 2019, 이석진, 강찬. All rights reserved.

#include "SaucewichAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

void USaucewichAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	APawn* Owner = TryGetPawnOwner();
	if (Owner)
	{
		FVector Velocity{ Owner->GetVelocity() };
		FRotator ActorRot = Owner->GetActorRotation();
		MoveDirection = FRotator::NormalizeAxis(Velocity.Rotation().Yaw - ActorRot.Yaw);
		MoveSpeed = Velocity.Size2D();

		FRotator AimRot = Owner->GetBaseAimRotation();
		AimPitch = AimRot.Pitch;

		float NewAimYaw = FRotator::NormalizeAxis(AimRot.Yaw - ActorRot.Yaw);
		if (FMath::IsNearlyZero(AimYawOffset, 179.f) && FMath::Abs(AimYaw - NewAimYaw) > 179.f)
		{
			AimYawOffset = AimYaw - NewAimYaw;
		}
		if (AimYawOffset != 0.f)
		{
			AimYaw = NewAimYaw + AimYawOffset;
			AimYawOffset = FMath::FInterpTo(AimYawOffset, 0.f, DeltaTime, AimYawInterpSpeed);
		}
		else
		{
			AimYaw = NewAimYaw;
		}

		if (AimYaw > 90.f)
		{
			HeadYaw = FMath::Clamp(AimYaw - 90.f, MinHeadYaw, MaxHeadYaw);
			AimYaw = 90.f;
		}
		else if (AimYaw < -90.f)
		{
			HeadYaw = FMath::Clamp(AimYaw + 90.f, MinHeadYaw, MaxHeadYaw);
			AimYaw = -90.f;
		}
		else
		{
			HeadYaw = 0.f;
		}

		bFalling = Owner->GetMovementComponent()->IsFalling();
	}
}
