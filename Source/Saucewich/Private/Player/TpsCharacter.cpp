// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "TpsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/SpringArmComponent.h"
#include "WeaponComponent.h"

ATpsCharacter::ATpsCharacter()
	:WeaponComponent{ CreateDefaultSubobject<UWeaponComponent>("WeaponComponent") },
	SpringArm{ CreateDefaultSubobject<USpringArmComponent>("SpringArm") },
	Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	Shadow{ CreateDefaultSubobject<UStaticMeshComponent>("Shadow") }
{
	WeaponComponent->SetupAttachment(GetMesh(), "Weapon");
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	Shadow->SetupAttachment(RootComponent);
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();
	ShadowData.Material = Shadow->CreateDynamicMaterialInstance(0);
}

void ATpsCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateShadow();
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	Input->BindAxis("MoveForward", this, &ATpsCharacter::MoveForward);
	Input->BindAxis("MoveRight", this, &ATpsCharacter::MoveRight);
	Input->BindAxis("Turn", this, &ATpsCharacter::AddControllerYawInput);
	Input->BindAxis("LookUp", this, &ATpsCharacter::AddControllerPitchInput);

	WeaponComponent->SetupPlayerInputComponent(Input);
}

void ATpsCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(GetActorRightVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::UpdateShadow()
{
	auto Start = GetActorLocation();
	Start.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	auto End = Start;
	End.Z -= ShadowData.MaxDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	const auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	Shadow->SetVisibility(bHit);
	if (bHit)
	{
		Hit.Location.Z += .01f;
		Shadow->SetWorldLocationAndRotation(
			Hit.Location,
			Hit.Normal.RotateAngleAxis(90.f, FVector::RightVector).Rotation()
		);
		ShadowData.Material->SetScalarParameterValue("Darkness", (1.f - (Start.Z - Hit.Location.Z) / ShadowData.MaxDistance) * ShadowData.Darkness);
	}
}
