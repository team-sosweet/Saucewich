// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

#include "Saucewich.h"
#include "Entity/Perk.h"
#include "Online/SaucewichGameMode.h"
#include "Online/SaucewichGameState.h"
#include "Player/CharacterData.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacterMovementComponent.h"
#include "Weapon/WeaponComponent.h"
#include "ShadowComponent.h"

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
	return GUARANTEE(WeaponComponent != nullptr) ? WeaponComponent->GetActiveWeapon() : nullptr;
}

EGunTraceHit ATpsCharacter::GunTrace(FHitResult& OutHit) const
{
	return GUARANTEE(WeaponComponent != nullptr) ? WeaponComponent->GunTrace(OutHit) : EGunTraceHit::None;
}

uint8 ATpsCharacter::GetTeam() const
{
	const auto Player = GetPlayerState<ASaucewichPlayerState>();
	return Player ? Player->GetTeam() : 0;
}

FLinearColor ATpsCharacter::GetColor() const
{
	FLinearColor Color;
	GUARANTEE(ColMat->GetVectorParameterValue({"Color"}, Color));
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
	ColMat->SetVectorParameterValue("Color", NewColor);
	if (GUARANTEE(ColTranslMat != nullptr)) ColTranslMat->SetVectorParameterValue("Color", NewColor);
	WeaponComponent->SetColor(NewColor);
}

bool ATpsCharacter::IsInvincible() const
{
	return GetWorldTimerManager().GetTimerRemaining(RespawnInvincibleTimerHandle) > 0;
}

void ATpsCharacter::SetMaxHP(const float Ratio)
{
	if (!GUARANTEE(Data != nullptr)) return;
	
	const auto OldMaxHP = MaxHP;
	MaxHP = Data->DefaultMaxHP * Ratio;
	if (HP == OldMaxHP) HP = MaxHP;
}

void ATpsCharacter::AddPerk(const TSubclassOf<APerk> PerkClass)
{
	if (const auto Class = *PerkClass)
	{
		MulticastAddPerk(Class);
	}
}

bool ATpsCharacter::HasPerk(const TSubclassOf<APerk> PerkClass) const
{
	return Perks.Contains(PerkClass);
}

float ATpsCharacter::GetSpeedRatio_Implementation() const
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

	if (GUARANTEE(Data != nullptr))
	{
		const auto ColMatIdx = GetColIdx();
		if (ColMatIdx != INDEX_NONE)
		{
			const auto Transl = Data->GetTranslMat(ColMatIdx, GetMesh()->GetMaterial(ColMatIdx));
			if (GUARANTEE(Transl != nullptr))
			{
				ColTranslMat = UMaterialInstanceDynamic::Create(Transl, GetMesh());
			}

			ColMat = GetMesh()->CreateDynamicMaterialInstance(ColMatIdx);
		}
	}
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PC = GetController<ASaucewichPlayerController>())
	{
		ASaucewichPlayerController::BroadcastCharacterSpawned(PC, this);
	}

	BindOnTeamChanged();

	if (HasAuthority())
	{
		bAlive = true;
		if (GUARANTEE(Data != nullptr))
		{
			HP = MaxHP = Data->DefaultMaxHP;
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

float ATpsCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	if (DamageAmount > 0) DamageAmount *= 1 - FMath::Clamp(GetArmor(), 0.f, 1.f);
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (HasAuthority() && DamageAmount != 0)
	{
		HP = FMath::Clamp(HP - DamageAmount, 0.f, MaxHP);
		if (HP == 0) Kill(EventInstigator->GetPlayerState<ASaucewichPlayerState>(), DamageCauser);
	}
	return DamageAmount;
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
		return DamageAmount > 0 ? GetTeam() != InstigatorState->GetTeam() : GetTeam() == InstigatorState->GetTeam();

	return true;
}

void ATpsCharacter::SetPlayerDefaults()
{
	HP = MaxHP;
	bAlive = true;
	SetActorActivated(true);

	if (GUARANTEE(Data != nullptr) && Data->RespawnInvincibleTime > 0)
	{
		BeTranslucent();
		GetWorldTimerManager().SetTimer(
			RespawnInvincibleTimerHandle, 
			this, &ATpsCharacter::BeOpaque,
			Data->RespawnInvincibleTime, false
		);
	}
	
	OnCharacterSpawn.Broadcast();
}

float ATpsCharacter::GetArmor_Implementation() const
{
	return 0;
}

// 주의: Server와 Client 모두에서 실행되지만, Client에서는 Attacker, Inflictor가 항상 nullptr입니다.
void ATpsCharacter::Kill(ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	HP = 0;
	bAlive = false;
	SetActorActivated(false);

	if (const auto GameMode = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
		if (const auto PC = GetController<ASaucewichPlayerController>())
			GameMode->SetPlayerRespawnTimer(PC);

	WeaponComponent->OnCharacterDeath();
	OnCharacterDeath.Broadcast();
	
	if (HasAuthority())
		if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
			GameState->MulticastPlayerDeath(GetPlayerState<ASaucewichPlayerState>(), Attacker, Inflictor);
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
	if (const auto PS = GetPlayerState())
	{
		if (const auto Player = Cast<ASaucewichPlayerState>(PS))
		{
			Player->OnTeamChangedDelegate.AddDynamic(this, &ATpsCharacter::OnTeamChanged);
			OnTeamChanged(Player->GetTeam());
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
		const auto GS = Cast<ASaucewichGameState>(GameState);
		if (GUARANTEE_MSG(GS != nullptr, "GameState를 SaucewichGameState로 변환 실패"))
		{
			SetColor(GetTeamColor(GS));
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

int32 ATpsCharacter::GetColIdx() const
{
	return GUARANTEE(Data != nullptr) ? GetMesh()->GetMaterialIndex(Data->ColMatName) : INDEX_NONE;
}

void ATpsCharacter::OnRep_Alive()
{
	if (bAlive) SetPlayerDefaults();
	else Kill();
}

void ATpsCharacter::BeTranslucent()
{
	if (bTranslucent) return;
	if (!Data) return;

	const auto ColMatIdx = GetColIdx();
	const auto NumMat = GetMesh()->GetNumMaterials();
	for (auto i = 0; i < NumMat; ++i)
	{
		if (i == ColMatIdx)
			GetMesh()->SetMaterial(i, ColTranslMat);
		
		else if (const auto Transl = Data->GetTranslMat(i, GetMesh()->GetMaterial(i)))
			GetMesh()->SetMaterial(i, Transl);
	}

	WeaponComponent->BeTranslucent();
	Shadow->BeTranslucent();
	
	bTranslucent = true;
}

void ATpsCharacter::BeOpaque()
{
	if (!bTranslucent) return;

	const auto* const DefMesh = GetDefault<ACharacter>(GetClass())->GetMesh();
	const auto ColMatIdx = GetColIdx();
	const auto NumMat = GetMesh()->GetNumMaterials();
	
	for (auto i = 0; i < NumMat; ++i)
		GetMesh()->SetMaterial(i, i == ColMatIdx ? ColMat : DefMesh->GetMaterial(i));

	WeaponComponent->BeOpaque();
	Shadow->BeOpaque();
	
	bTranslucent = false;
}

void ATpsCharacter::MulticastAddPerk_Implementation(UClass* const PerkClass)
{
	check(PerkClass);
	
	const auto Def = GetDefault<APerk>(PerkClass);
	auto& Perk = Perks.FindOrAdd(PerkClass);

	if (!Perk.PSC && Def->GetParticle())
	{
		Perk.PSC = UGameplayStatics::SpawnEmitterAttached(
			Def->GetParticle(),
			GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FVector::OneVector,
			EAttachLocation::KeepRelativeOffset,
			true,
			EPSCPoolMethod::ManualRelease
		);
	}

	auto&& ErasePerk = [this, PerkClass]
	{
		if (const auto Found = Perks.Find(PerkClass))
			if (Found->PSC) Found->PSC->ReleaseToPool();
		
		Perks.Remove(PerkClass);
	};

	GetWorldTimerManager().SetTimer(Perk.Timer, ErasePerk, Def->GetDuration(),	false);
}
