// Copyright (c) 2019, 이석진, 강찬. All rights reserved.

#include "SaucewichCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon.h"

//////////////////////////////////////////////////////////////////////////
// ASaucewichCharacter

ASaucewichCharacter::ASaucewichCharacter()
	:CameraBoom{ CreateDefaultSubobject<USpringArmComponent>("CameraBoom") },
	FollowCamera{ CreateDefaultSubobject<UCameraComponent>("FollowCamera") }
{
	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

//////////////////////////////////////////////////////////////////////////
// Weapon

void ASaucewichCharacter::GiveWeapon(AWeapon * Weapon)
{
	if (Weapon)
	{
		this->Weapon = Weapon;
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Weapon");
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASaucewichCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &ASaucewichCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASaucewichCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASaucewichCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASaucewichCharacter::LookUpAtRate);
}

void ASaucewichCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASaucewichCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASaucewichCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASaucewichCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}
