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
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "SaucewichGameState.h"
#include "SaucewichPlayerController.h"
#include "SaucewichPlayerState.h"
#include "TpsCharacterMovementComponent.h"
#include "WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogTpsCharacter, Log, All)

ATpsCharacter::ATpsCharacter(const FObjectInitializer& ObjectInitializer)
	:Super{ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(CharacterMovementComponentName)},
	WeaponComponent{ CreateDefaultSubobject<UWeaponComponent>("WeaponComponent") },
	SpringArm{ CreateDefaultSubobject<USpringArmComponent>("SpringArm") },
	Camera{ CreateDefaultSubobject<UCameraComponent>("Camera") },
	Shadow{ CreateDefaultSubobject<UStaticMeshComponent>("Shadow") }
{
	WeaponComponent->SetupAttachment(GetMesh(), "Weapon");
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	Shadow->SetupAttachment(RootComponent);
}

AWeapon* ATpsCharacter::GetActiveWeapon() const
{
	return WeaponComponent->GetActiveWeapon();
}

EGunTraceHit ATpsCharacter::GunTrace(FHitResult& OutHit) const
{
	return WeaponComponent->GunTrace(OutHit);
}

uint8 ATpsCharacter::GetTeam() const
{
	const auto Player = GetPlayerState<ASaucewichPlayerState>();
	return Player ? Player->GetTeam() : 0;
}

FLinearColor ATpsCharacter::GetColor() const
{
	FLinearColor Color;
	Material->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

FLinearColor ATpsCharacter::GetTeamColor() const
{
	if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		return GameState->GetTeamData(GetTeam()).Color;
	}
	return {};
}

void ATpsCharacter::SetColor(const FLinearColor& NewColor)
{
	Material->SetVectorParameterValue("Color", NewColor);
	WeaponComponent->SetColor(NewColor);
}

void ATpsCharacter::SetMaxHP(const float Ratio)
{
	const auto OldMaxHP = MaxHP;
	MaxHP = DefaultMaxHP * Ratio;
	if (HP == OldMaxHP) HP = MaxHP;
}

float ATpsCharacter::GetSpeedRatio() const
{
	return WeaponComponent->GetSpeedRatio();
}

FVector ATpsCharacter::GetPawnViewLocation() const
{
	if (Role == ROLE_SimulatedProxy)
	{
		const auto ArmLocation = GetSpringArmLocation();
		return ArmLocation - GetBaseAimRotation().Vector() * (ArmLocation - Camera->GetComponentLocation()).Size();
	}
	return Camera->GetComponentLocation();
}

FVector ATpsCharacter::GetSpringArmLocation() const
{
	return SpringArm->GetComponentLocation();
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	HP = MaxHP;
	ShadowData.Material = Shadow->CreateDynamicMaterialInstance(0);
	BindOnTeamChanged();
}

void ATpsCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateShadow();
}

void ATpsCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Material = GetMesh()->CreateDynamicMaterialInstance(FMath::Max(GetMesh()->GetMaterialIndex("TeamColor"), 0));
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

void ATpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATpsCharacter, HP);
	DOREPLIFETIME(ATpsCharacter, MaxHP);
}

float ATpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage != 0.f)
	{
		HP = FMath::Clamp(HP - Damage, 0.f, MaxHP);
		if (HP == 0.f)
		{
			Kill();
		}
	}
	return Damage;
}

bool ATpsCharacter::ShouldTakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser) const
{
	if (!Super::ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser))
		return false;

	if (!EventInstigator)
		return true;

	if (const auto State = EventInstigator->GetPlayerState<ASaucewichPlayerState>())
		return DamageAmount > 0.f ? GetTeam() != State->GetTeam() : GetTeam() == State->GetTeam();

	return true;
}

void ATpsCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(GetActorRightVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::OnTeamChanged(const uint8 NewTeam)
{
	SetColor(GetTeamColor());
}

void ATpsCharacter::BindOnTeamChanged()
{
	if (const auto PlayerState = GetPlayerState())
	{
		if (const auto MyState = Cast<ASaucewichPlayerState>(PlayerState))
		{
			MyState->OnTeamChangedDelegate.AddDynamic(this, &ATpsCharacter::OnTeamChanged);
			OnTeamChanged(MyState->GetTeam());
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATpsCharacter::BindOnTeamChanged);
	}
}

void ATpsCharacter::UpdateShadow() const
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
