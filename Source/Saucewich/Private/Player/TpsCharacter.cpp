// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "SaucewichGameMode.h"
#include "SaucewichGameState.h"
#include "SaucewichPlayerController.h"
#include "SaucewichPlayerState.h"
#include "ShadowComponent.h"
#include "TpsCharacterMovementComponent.h"
#include "TranslMatData.h"
#include "WeaponComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogTpsCharacter, Log, All)

ATpsCharacter::ATpsCharacter(const FObjectInitializer& ObjectInitializer)
	:Super{ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(CharacterMovementComponentName)},
	WeaponComponent{CreateDefaultSubobject<UWeaponComponent>("WeaponComponent")},
	SpringArm{CreateDefaultSubobject<USpringArmComponent>("SpringArm")},
	Camera{CreateDefaultSubobject<UCameraComponent>("Camera")},
	Shadow{CreateDefaultSubobject<UShadowComponent>("Shadow")}
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
	return State ? State->GetTeam() : 0;
}

FLinearColor ATpsCharacter::GetColor() const
{
	FLinearColor Color;
	if (const auto Mat = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(GetMesh()->GetMaterialIndex("TeamColor"))))
		Mat->GetVectorParameterValue({"Color"}, Color);
	return Color;
}

FLinearColor ATpsCharacter::GetTeamColor() const
{
	if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
	{
		return GetTeamColor(GameState);
	}
	return {};
}

void ATpsCharacter::SetColor(const FLinearColor& NewColor)
{
	if (const auto Mat = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(GetMesh()->GetMaterialIndex("TeamColor"))))
		Mat->SetVectorParameterValue("Color", NewColor);
	WeaponComponent->SetColor(NewColor);
}

bool ATpsCharacter::IsInvincible() const
{
	return GetWorldTimerManager().GetTimerRemaining(RespawnInvincibleTimerHandle) > 0.f;
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

void ATpsCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const auto Idx = FMath::Max(GetMesh()->GetMaterialIndex("TeamColor"), 0);
	DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(Idx);

	RegisterGameMode();
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindOnTeamChanged();

	if (HasAuthority())
	{
		bAlive = true;
		HP = MaxHP = DefaultMaxHP;

		if (State)
		{
			if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
			{
				if (!GS->IsValidTeam(GetTeam()))
				{
					State->SetTeam(GS->GetMinPlayerTeam());
				}
			}
		}
	}
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	Input->BindAxis("MoveForward", this, &ATpsCharacter::MoveForward);
	Input->BindAxis("MoveRight", this, &ATpsCharacter::MoveRight);
	Input->BindAxis("Turn", this, &ATpsCharacter::AddControllerYawInput);
	Input->BindAxis("LookUp", this, &ATpsCharacter::AddControllerPitchInput);
	Input->BindAction("Respawn", IE_Pressed, this, &ATpsCharacter::Respawn);

	WeaponComponent->SetupPlayerInputComponent(Input);
}

void ATpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATpsCharacter, HP);
	DOREPLIFETIME(ATpsCharacter, MaxHP);
	DOREPLIFETIME(ATpsCharacter, bAlive);
}

float ATpsCharacter::TakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (HasAuthority() && DamageAmount != 0)
	{
		HP = FMath::Clamp(HP - Damage, 0.f, MaxHP);
		if (HP == 0) Kill(EventInstigator->GetPlayerState<ASaucewichPlayerState>(), DamageCauser);
	}
	return Damage;
}

bool ATpsCharacter::ShouldTakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser) const
{
	if (!bCanBeDamaged)
		return false;

	if (!IsAlive())
		return false;

	if (IsInvincible())
		return false;

	if (FMath::IsNearlyZero(DamageAmount))
		return false;

	if (!EventInstigator)
		return true;

	if (const auto InstigatorState = EventInstigator->GetPlayerState<ASaucewichPlayerState>())
		return DamageAmount > 0.f ? GetTeam() != InstigatorState->GetTeam() : GetTeam() == InstigatorState->GetTeam();

	return true;
}

void ATpsCharacter::SetPlayerDefaults()
{
	HP = MaxHP;
	bAlive = true;
	SetActorActivated(true);

	if (RespawnInvincibleTime > 0.f)
	{
		BeTranslucent();
		GetWorldTimerManager().SetTimer(
			RespawnInvincibleTimerHandle, 
			this, &ATpsCharacter::BeOpaque,
			RespawnInvincibleTime, false
		);
	}
	
	OnCharacterSpawn.Broadcast();
}

// 주의: Server와 Client 모두에서 실행되지만, Client에서는 Attacker, Inflictor가 항상 nullptr입니다.
void ATpsCharacter::Kill(ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	HP = 0.f;
	bAlive = false;
	SetActorActivated(false);

	if (GameMode)
		if (const auto PC = GetController<ASaucewichPlayerController>())
			GameMode->SetPlayerRespawnTimer(PC);

	State->OnDeath();
	WeaponComponent->OnCharacterDeath();
	OnCharacterDeath.Broadcast();

	if (HasAuthority())
		if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
			GameState->MulticastPlayerDeath(State, Attacker, Inflictor);
}

void ATpsCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(GetActorRightVector(), FMath::Sign(AxisValue));
}

void ATpsCharacter::Respawn()
{
	if (IsAlive()) return;
	if (const auto PC = GetController<ASaucewichPlayerController>())
	{
		PC->Respawn();
	}
}

void ATpsCharacter::SetActorActivated(const bool bActive)
{
	SetActorTickEnabled(bActive);
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
	GetCharacterMovement()->SetComponentTickEnabled(bActive);
}

void ATpsCharacter::OnTeamChanged(const uint8 NewTeam)
{
	SetColorToTeamColor();
}

void ATpsCharacter::BindOnTeamChanged()
{
	if (const auto PState = GetPlayerState())
	{
		State = Cast<ASaucewichPlayerState>(PState);
		if (State)
		{
			State->OnTeamChangedDelegate.AddDynamic(this, &ATpsCharacter::OnTeamChanged);
			OnTeamChanged(State->GetTeam());
		}
		else
		{
			UE_LOG(LogTpsCharacter, Error, TEXT("PlayerState -> SaucewichPlayerState 변환에 실패했습니다. 일부 기능이 작동하지 않을 수 있습니다."));
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATpsCharacter::BindOnTeamChanged);
	}
}

void ATpsCharacter::SetColorToTeamColor()
{
	if (const auto GameState = GetWorld()->GetGameState())
	{
		if (const auto GS = Cast<ASaucewichGameState>(GameState))
		{
			SetColor(GetTeamColor(GS));
		}
		else
		{
			UE_LOG(LogTpsCharacter, Error, TEXT("GameState가 있긴 한데 ASaucewichGameState가 아닙니다!"));
		}
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATpsCharacter::SetColorToTeamColor);
	}
}

FLinearColor ATpsCharacter::GetTeamColor(ASaucewichGameState* const GameState) const
{
	return GameState->GetTeamData(GetTeam()).Color;
}

void ATpsCharacter::OnRep_Alive()
{
	if (bAlive) SetPlayerDefaults();
	else Kill();
}

void ATpsCharacter::RegisterGameMode()
{
	if (!HasAuthority()) return;
	GameMode = GetWorld()->GetAuthGameMode<ASaucewichGameMode>();
}

void ATpsCharacter::BeTranslucent()
{
	if (bTranslucent || !TranslMatData) return;

	const auto Colored = GetMesh()->GetMaterialIndex("TeamColor");
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		const auto Ptr = TranslMatData->TranslMatByIdx.Find(i);
		const auto Mat = Ptr ? *Ptr : TranslMatData->DefTranslMat;

		if (i == Colored)
		{
			const auto Color = GetColor();
			GetMesh()->CreateDynamicMaterialInstance(i, Mat)->SetVectorParameterValue("Color", Color);
		}
		else
		{
			GetMesh()->SetMaterial(i, Mat);
		}
	}

	WeaponComponent->BeTranslucent();
	Shadow->BeTranslucent();
	bTranslucent = true;
}

void ATpsCharacter::BeOpaque()
{
	if (!bTranslucent) return;

	const auto* const DefMesh = GetDefault<ACharacter>(GetClass())->GetMesh();
	const auto Colored = GetMesh()->GetMaterialIndex("TeamColor");
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		if (i == Colored)
		{
			FLinearColor Color;
			GetMesh()->GetMaterial(i)->GetVectorParameterValue({"Color"}, Color);
			DynamicMaterial->SetVectorParameterValue("Color", Color);
			GetMesh()->SetMaterial(i, DynamicMaterial);
		}
		else
		{
			GetMesh()->SetMaterial(i, DefMesh->GetMaterial(i));
		}
	}

	WeaponComponent->BeOpaque();
	Shadow->BeOpaque();
	bTranslucent = false;
}
