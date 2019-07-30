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
#include "UnrealNetwork.h"

#include "SaucewichGameState.h"
#include "SaucewichPlayerController.h"
#include "SaucewichPlayerState.h"
#include "WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogTpsCharacter, Log, All)

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
	if (!Material) 
	{
		UE_LOG(LogTpsCharacter, Error, TEXT("Tried to get color before material created!"));
		return {};
	}
	FLinearColor Color;
	Material->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

void ATpsCharacter::SetColor(const FLinearColor& NewColor)
{
	Material->SetVectorParameterValue("Color", NewColor);
	WeaponComponent->SetColor(NewColor);
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

	Hp = MaxHp;
	ShadowData.Material = Shadow->CreateDynamicMaterialInstance(0);

	if (const auto Player = GetPlayerState<ASaucewichPlayerState>())
	{
		BindOnTeamChanged(Player);
	}
	else if (const auto MyController = GetController<ASaucewichPlayerController>())
	{
		MyController->OnPlayerStateSpawned.AddDynamic(this, &ATpsCharacter::BindOnTeamChanged);
	}
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

	DOREPLIFETIME(ATpsCharacter, Hp);
}

float ATpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage != 0.f)
	{
		Hp = FMath::Clamp(Hp - Damage, 0.f, MaxHp);
		if (Hp == 0.f)
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
		return GetTeam() != State->GetTeam();

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

void ATpsCharacter::BindOnTeamChanged(ASaucewichPlayerState* const Player)
{
	Player->OnTeamChanged.AddDynamic(this, &ATpsCharacter::OnTeamChanged);
	OnTeamChanged(Player->GetTeam());
}

void ATpsCharacter::OnTeamChanged(const uint8 NewTeam)
{
	if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		SetColor(GameState->GetTeamData(NewTeam).Color);
	}
}
