// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "SaucewichCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UnrealNetwork.h"
#include "Weapon.h"

DECLARE_LOG_CATEGORY_CLASS(LogSaucewichCharacter, Log, All)

ASaucewichCharacter::ASaucewichCharacter()
	:CameraBoom{ CreateDefaultSubobject<USpringArmComponent>("CameraBoom") },
	FollowCamera{ CreateDefaultSubobject<UCameraComponent>("FollowCamera") }
{
	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void ASaucewichCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	static const FName SlotName{ "Color" };
	ClothColorDynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(GetMesh()->GetMaterialIndex(SlotName)), this);
	GetMesh()->SetMaterialByName(SlotName, ClothColorDynamicMaterial);
}

void ASaucewichCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurnWhenNotMoving();
	ReplicateView();
	GetPawnViewLocation();
	PostTick.Broadcast(DeltaTime);
}

void ASaucewichCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASaucewichCharacter, Weapon);
	DOREPLIFETIME(ASaucewichCharacter, HP);
	DOREPLIFETIME_CONDITION(ASaucewichCharacter, RemoteViewYaw, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ASaucewichCharacter, RemoteViewLocation, COND_SimulatedOnly);
}

float ASaucewichCharacter::TakeDamage(const float Damage, FDamageEvent const& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	if (Role != ROLE_Authority || !Alive()) return 0.f;

	const auto ActualDamage{ Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser) };
	HP = FMath::Clamp(HP - ActualDamage, 0.f, GetClass()->GetDefaultObject<ASaucewichCharacter>()->HP);
	OnHPChanged();
	return ActualDamage;
}

void ASaucewichCharacter::OnHPChanged()
{
	if (HP <= 0.f)
	{
		Kill();
	}
}

void ASaucewichCharacter::Kill()
{
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void ASaucewichCharacter::GiveWeapon(const FDataTableRowHandle& WeaponData)
{
	if (const auto* const Data{ WeaponData.GetRow<FWeaponData>(TEXT(__FUNCTION__)) })
	{
		if (Data->BaseClass)
		{
			FActorSpawnParameters Param;
			Param.Owner = this;
			Param.Instigator = this;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			if (const auto NewWeapon{ GetWorld()->SpawnActor<AWeapon>(Data->BaseClass, Param) })
			{
				auto& OldWeapon{ Weapon[static_cast<uint8>(Data->Position)] };
				if (OldWeapon)
				{
					OldWeapon->Destroy();
				}
				OldWeapon = NewWeapon;
				NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "Weapon");
				NewWeapon->Equip(Data, WeaponData.RowName);
				NewWeapon->SetColor(GetColor());

				if (Data->Position == EWeaponPosition::Primary)
				{
					const auto DefaultSpeed{ GetClass()->GetDefaultObject<ACharacter>()->GetCharacterMovement()->MaxWalkSpeed };
					GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed - FMath::Clamp(Data->Weight, 0.f, DefaultSpeed);
				}
			}
		}
		else
		{
			UE_LOG(LogSaucewichCharacter, Error, TEXT(__FUNCTION__" : Failed to give weapon. Weapon's BaseClass hasn't been set in data table."));
		}
	}
}

void ASaucewichCharacter::WeaponAttack()
{
	if (Weapon[ActiveWeaponIdx])
	{
		Weapon[ActiveWeaponIdx]->StartAttack();
	}
}

void ASaucewichCharacter::WeaponStopAttack()
{
	if (Weapon[ActiveWeaponIdx])
	{
		Weapon[ActiveWeaponIdx]->StopAttack();
	}
}

const FName ParameterName{ "TeamColor" };

void ASaucewichCharacter::SetColor(const FLinearColor& Color)
{
	ClothColorDynamicMaterial->SetVectorParameterValue(ParameterName, Color);
}

FLinearColor ASaucewichCharacter::GetColor() const
{
	FLinearColor Color;
	ClothColorDynamicMaterial->GetVectorParameterValue(ParameterName, Color);
	return Color;
}

void ASaucewichCharacter::TurnWhenNotMoving()
{
	if (Role == ROLE_SimulatedProxy) return;

	const auto bMoving{ GetVelocity().Size() > 0.f };
	GetCharacterMovement()->bUseControllerDesiredRotation = bMoving;

	if (!IsLocallyControlled()) return;

	if (!bMoving)
	{
		EDirection TurnDirection;
		const auto bShouldTurn{ CheckShouldTurn(TurnDirection) };
		if (bShouldTurn)
		{
			StartTurn(TurnDirection);
		}
	}
}

bool ASaucewichCharacter::CheckShouldTurn(EDirection& OutDirection)
{
	if (DoTurn.IsValid()) return false;

	const auto Diff{ FRotator::NormalizeAxis(GetActorRotation().Yaw - GetBaseAimRotation().Yaw) };
	const auto bShouldTurn{ FMath::Abs(Diff) > TurnAnimRate };
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
	const auto TurnTime{ TurnAnim->SequenceLength / TurnAnim->RateScale / 2.f };
	const auto YawDelta{ 90.f * ((Direction == EDirection::Right) ? 1.f : -1.f) };
	const auto OldRotation{ GetActorRotation() };
	const auto NewRotation{ OldRotation + FRotator{ 0.f, YawDelta, 0.f } };

	TurnAlpha = 0.f;
	DoTurn = PostTick.AddLambda([this, TurnTime, OldRotation, NewRotation](const float DeltaTime)
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
	PlayAnimMontage(TurnAnim, 1.f, (Direction == EDirection::Left) ? "Left" : "Right");
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

FVector ASaucewichCharacter::GetPawnViewLocation() const
{
	return Role == ROLE_SimulatedProxy ? RemoteViewLocation : FollowCamera->GetComponentLocation();
}

FRotator ASaucewichCharacter::GetBaseAimRotation() const
{
	auto BaseRotation = Super::GetBaseAimRotation();
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
		const auto Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };

		const auto Direction{ FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void ASaucewichCharacter::MoveRight(const float Value)
{
	if (Controller && Value != 0.0f)
	{
		const auto Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0.f };
	
		const auto Direction{ FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}
