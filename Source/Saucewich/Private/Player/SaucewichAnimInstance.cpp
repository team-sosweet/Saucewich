// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SaucewichAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SaucewichCharacter.h"
#include "Weapon.h"

void USaucewichAnimInstance::NativeUpdateAnimation(const float DeltaTime)
{
	if (const auto Owner{ TryGetPawnOwner() })
	{
		const auto Velocity{ Owner->GetVelocity() };
		const auto OwnerRot{ Owner->GetActorRotation() };

		MoveDirection = FRotator::NormalizeAxis(Velocity.Rotation().Yaw - OwnerRot.Yaw);
		MoveSpeed = Velocity.Size2D();

		AimOffset = Owner->GetBaseAimRotation() - OwnerRot;
		AimOffset.Normalize();

		HeadYaw = 0.f;
		if (FMath::Abs(AimOffset.Yaw) > 90.f)
		{
			HeadYaw = FMath::Clamp(AimOffset.Yaw - 90.f * FMath::Sign(AimOffset.Yaw), MinHeadYaw, MaxHeadYaw);
			AimOffset.Yaw = FMath::Clamp(AimOffset.Yaw, -90.f, 90.f);
		}

		bFalling = Owner->GetMovementComponent()->IsFalling();

		if (const auto Character{ Cast<ASaucewichCharacter>(Owner) })
		{
			if (const auto Weapon{ Character->GetActiveWeapon() })
			{
				if (const auto Data{ Weapon->GetData() })
				{
					WeaponGripType = Data->GripType;
				}
			}
		}
	}
}
