// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SaucewichCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UnrealNetwork.h"
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

void ASaucewichCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnWhenNotMoving();
	ReplicateCameraYaw();

	PostTick.Broadcast(DeltaTime);
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
// Turn when not moving

void ASaucewichCharacter::TurnWhenNotMoving()
{
	if (Role == ROLE_SimulatedProxy) return;

	bool bMoving = GetVelocity().Size() > 0.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = bMoving;

	if (!IsLocallyControlled()) return;

	if (!bMoving)
	{
		EDirection TurnDirection;
		bool bShouldTurn = CheckShouldTurn(TurnDirection);
		if (bShouldTurn)
		{
			StartTurn(TurnDirection);
		}
	}
}

bool ASaucewichCharacter::CheckShouldTurn(EDirection& OutDirection)
{
	if (DoTurn.IsValid()) return false;

	float Diff = FRotator::NormalizeAxis(GetActorRotation().Yaw - GetBaseAimRotation().Yaw);
	bool bShouldTurn = FMath::Abs(Diff) > TurnAnimRate;
	if (bShouldTurn)
	{
		OutDirection = Diff < 0.f ? EDirection::Right : EDirection::Left;
		return true;
	}

	return false;
}

void ASaucewichCharacter::StartTurn(EDirection Direction)
{
	ServerStartTurn(Direction);

	if (Role != ROLE_Authority)
	{
		StartTurn_Internal(Direction);
	}
}

void ASaucewichCharacter::StartTurn_Internal(EDirection Direction)
{
	float TurnTime = TurnAnim->SequenceLength / TurnAnim->RateScale / 2.f;
	TurnAlpha = 0.f;

	FRotator OldRotation = GetActorRotation();
	FRotator NewRotation = OldRotation;
	NewRotation.Yaw += Direction == EDirection::Right ? 90.f : -90.f;
	
	DoTurn = PostTick.AddLambda([=](float DeltaTime)
	{
		TurnAlpha += DeltaTime / TurnTime;
		if (TurnAlpha >= 1.f)
		{
			PostTick.Remove(DoTurn);
			DoTurn.Reset();
		}
		else
		{
			SetActorRotation(FMath::Lerp(OldRotation, NewRotation, TurnAlpha));
		}
	});

	SimulateTurn(Direction);
}

void ASaucewichCharacter::SimulateTurn(EDirection Direction)
{
	PlayAnimMontage(TurnAnim, 1.f, Direction == EDirection::Left ? "Left" : "Right");
}

void ASaucewichCharacter::ServerStartTurn_Implementation(EDirection Direction)
{
	MulticastSimulateTurn(Direction);
	StartTurn_Internal(Direction);
}
bool ASaucewichCharacter::ServerStartTurn_Validate(EDirection) { return true; }

void ASaucewichCharacter::MulticastSimulateTurn_Implementation(EDirection Direction)
{
	if (Role != ROLE_AutonomousProxy)
	{
		SimulateTurn(Direction);
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication

void ASaucewichCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASaucewichCharacter, RemoteViewYaw, COND_SimulatedOnly);
}

FRotator ASaucewichCharacter::GetBaseAimRotation() const
{
	FRotator BaseRotation = Super::GetBaseAimRotation();
	if (Role == ROLE_SimulatedProxy)
	{
		BaseRotation.Yaw = FRotator::DecompressAxisFromByte(RemoteViewYaw);
	}
	return BaseRotation;
}

void ASaucewichCharacter::ReplicateCameraYaw()
{
	if (Role != ROLE_Authority) return;

	RemoteViewYaw = FRotator::CompressAxisToByte(FollowCamera->GetComponentRotation().Yaw);
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
