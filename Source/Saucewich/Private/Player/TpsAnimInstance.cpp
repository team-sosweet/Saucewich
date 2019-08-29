// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "TpsAnimInstance.h"
#include "Gun.h"
#include "TpsCharacter.h"

void UTpsAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	const auto Pawn = Cast<ATpsCharacter>(TryGetPawnOwner());
	if (!Pawn) return;

	const auto Velocity = Pawn->GetVelocity();
	Speed = Velocity.Size2D();

	auto bSetAimPitch = false;
	if (const auto Gun = Cast<AGun>(Pawn->GetActiveWeapon()))
	{
		FHitResult Hit;
		if (Gun->GunTrace(Hit) != EGunTraceHit::None)
		{
			AimPitch = ((Hit.ImpactPoint - Gun->GetActorLocation()).Rotation() - Pawn->GetActorRotation()).GetNormalized().Pitch * AimPitchMul;
			bSetAimPitch = true;
		}
	}
	if (!bSetAimPitch)
	{
		AimPitch = Pawn->GetBaseAimRotation().Pitch + AimPitchOffsetWhileNoTarget;
	}
}
