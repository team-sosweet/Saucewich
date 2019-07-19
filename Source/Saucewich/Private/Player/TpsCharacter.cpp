// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "TpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATpsCharacter::ATpsCharacter()
	:SpringArm{ CreateDefaultSubobject<USpringArmComponent>("SpringArm") },
	Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") }
{
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	Input->BindAxis("MoveForward", this, &ATpsCharacter::MoveForward);
	Input->BindAxis("MoveRight", this, &ATpsCharacter::MoveRight);
	Input->BindAxis("Turn", this, &ATpsCharacter::AddControllerYawInput);
	Input->BindAxis("LookUp", this, &ATpsCharacter::AddControllerPitchInput);
}

void ATpsCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(GetActorRightVector(), FMath::Sign(AxisValue));
}
