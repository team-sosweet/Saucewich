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

void ASaucewichCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnWhenNotMoving();
	ReplicateView();
	GetPawnViewLocation();
	PostTick.Broadcast(DeltaTime);
}

//////////////////////////////////////////////////////////////////////////
// Weapon

void ASaucewichCharacter::GiveWeapon(AWeapon* const NewWeapon)
{
	if (NewWeapon)
	{
		Weapon = NewWeapon;
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Weapon");
	}
}

void ASaucewichCharacter::WeaponAttack()
{
	if (Weapon)
	{
		Weapon->Attack();
	}
}

void ASaucewichCharacter::WeaponStopAttack()
{
	if (Weapon)
	{
		Weapon->StopAttack();
	}
}

//////////////////////////////////////////////////////////////////////////
// Turn when not moving

void ASaucewichCharacter::TurnWhenNotMoving()
{
	if (Role == ROLE_SimulatedProxy) return;

	const bool bMoving = GetVelocity().Size() > 0.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = bMoving;

	if (!IsLocallyControlled()) return;

	if (!bMoving)
	{
		EDirection TurnDirection;
		const bool bShouldTurn = CheckShouldTurn(TurnDirection);
		if (bShouldTurn)
		{
			StartTurn(TurnDirection);
		}
	}
}

bool ASaucewichCharacter::CheckShouldTurn(EDirection& OutDirection)
{
	if (DoTurn.IsValid()) return false;

	const float Diff = FRotator::NormalizeAxis(GetActorRotation().Yaw - GetBaseAimRotation().Yaw);
	const bool bShouldTurn = FMath::Abs(Diff) > TurnAnimRate;
	if (bShouldTurn)
	{
		OutDirection = (Diff < 0.f) ? EDirection::Right : EDirection::Left;
		return true;
	}

	return false;
}

void ASaucewichCharacter::StartTurn(const EDirection Direction)
{
	ServerStartTurn(Direction);

	if (Role != ROLE_Authority)
	{
		StartTurn_Internal(Direction);
	}
}

void ASaucewichCharacter::StartTurn_Internal(const EDirection Direction)
{
	const float TurnTime = TurnAnim->SequenceLength / TurnAnim->RateScale / 2.f;
	const float YawDelta = 90.f * ((Direction == EDirection::Right) ? 1.f : -1.f);
	const FRotator OldRotation = GetActorRotation();
	const FRotator NewRotation = OldRotation + FRotator{ 0.f, YawDelta, 0.f };

	TurnAlpha = 0.f;
	DoTurn = PostTick.AddLambda([=](const float DeltaTime)
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

	PlayTurnAnim(Direction);
}

void ASaucewichCharacter::PlayTurnAnim(const EDirection Direction)
{
	const FName Section = (Direction == EDirection::Left) ? "Left" : "Right";
	PlayAnimMontage(TurnAnim, 1.f, Section);
}

void ASaucewichCharacter::ServerStartTurn_Implementation(const EDirection Direction)
{
	MulticastSimulateTurn(Direction);
	StartTurn_Internal(Direction);
}
bool ASaucewichCharacter::ServerStartTurn_Validate(EDirection) { return true; }

void ASaucewichCharacter::MulticastSimulateTurn_Implementation(const EDirection Direction)
{
	if (Role != ROLE_AutonomousProxy)
	{
		PlayTurnAnim(Direction);
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication

void ASaucewichCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASaucewichCharacter, Weapon);
	DOREPLIFETIME_CONDITION(ASaucewichCharacter, RemoteViewYaw, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ASaucewichCharacter, RemoteViewLocation, COND_SimulatedOnly);
}

//////////////////////////////////////////////////////////////////////////
// Camera

FVector ASaucewichCharacter::GetPawnViewLocation() const
{
	return Role == ROLE_SimulatedProxy ? RemoteViewLocation : FollowCamera->GetComponentLocation();
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

void ASaucewichCharacter::ReplicateView()
{
	if (Role != ROLE_Authority) return;

	RemoteViewYaw = FRotator::CompressAxisToByte(FollowCamera->GetComponentRotation().Yaw);
	RemoteViewLocation = FollowCamera->GetComponentLocation();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASaucewichCharacter::SetupPlayerInputComponent(UInputComponent* const PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveForward", this, &ASaucewichCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASaucewichCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASaucewichCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASaucewichCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ASaucewichCharacter::WeaponAttack);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ASaucewichCharacter::WeaponStopAttack);
}

void ASaucewichCharacter::TurnAtRate(const float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASaucewichCharacter::LookUpAtRate(const float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASaucewichCharacter::MoveForward(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };

		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASaucewichCharacter::MoveRight(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };
	
		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}
