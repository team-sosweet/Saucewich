// Copyright 2019 Seokjin Lee. All Rights Reserved.

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
#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "Player/CharacterData.h"
#include "Player/SaucewichPlayerController.h"
#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacterMovementComponent.h"
#include "Weapon/WeaponComponent.h"
#include "ShadowComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All)

ATpsCharacter::ATpsCharacter(const FObjectInitializer& ObjectInitializer)
	:Super{ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(CharacterMovementComponentName)},
	WeaponComponent{CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"))},
	SpringArm{CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"))},
	Camera{CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"))},
	Shadow{CreateDefaultSubobject<UShadowComponent>(TEXT("Shadow"))}
{
	WeaponComponent->SetupAttachment(GetMesh(), TEXT("Weapon"));
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	Shadow->SetupAttachment(RootComponent);
}

AWeapon* ATpsCharacter::GetActiveWeapon() const
{
	return WeaponComponent ? WeaponComponent->GetActiveWeapon() : nullptr;
}

EGunTraceHit ATpsCharacter::GunTrace(FHitResult& OutHit) const
{
	return WeaponComponent ? WeaponComponent->GunTrace(OutHit) : EGunTraceHit::None;
}

uint8 ATpsCharacter::GetTeam() const
{
	const auto Player = GetPlayerState<ASaucewichPlayerState>();
	return Player ? Player->GetTeam() : 0;
}

FLinearColor ATpsCharacter::GetColor() const
{
	FLinearColor Color;
	ColMat->GetVectorParameterValue({TEXT("Color")}, Color);
	return Color;
}

const FLinearColor& ATpsCharacter::GetTeamColor() const
{
	return ASaucewichGameMode::GetData(this).Teams[GetTeam()].Color;
}

void ATpsCharacter::SetColor(const FLinearColor& NewColor)
{
	ColMat->SetVectorParameterValue(TEXT("Color"), NewColor);
	ColTranslMat->SetVectorParameterValue(TEXT("Color"), NewColor);
	WeaponComponent->SetColor(NewColor);
}

bool ATpsCharacter::IsInvincible() const
{
	return GetWorldTimerManager().GetTimerRemaining(RespawnInvincibleTimer) > 0;
}

void ATpsCharacter::AddPerk(const TSubclassOf<APerk> PerkClass)
{
	check(PerkClass);
	MulticastAddPerk(PerkClass);
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

	const auto ColMatIdx = GetColIdx();
	const auto Transl = Data->GetTranslMat(ColMatIdx, GetMesh()->GetMaterial(ColMatIdx));
	
	ColTranslMat = UMaterialInstanceDynamic::Create(Transl, GetMesh());
	ColMat = GetMesh()->CreateDynamicMaterialInstance(ColMatIdx);
}

void ATpsCharacter::PossessedBy(AController* const NewController)
{
	Super::PossessedBy(NewController);

	if (const auto PC = Cast<ASaucewichPlayerController>(NewController))
		ASaucewichPlayerController::BroadcastCharacterSpawned(PC, this);
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PC = GetController<ASaucewichPlayerController>())
		ASaucewichPlayerController::BroadcastCharacterSpawned(PC, this);
	
	BindOnTeamChanged();

	if (HasAuthority())
	{
		bAlive = true;
		if (Data != nullptr)
		{
			HP = Data->MaxHP;
		}
	}
}

void ATpsCharacter::Destroyed()
{
	if (const auto PS = GetPlayerState<ASaucewichPlayerState>())
	{
		PS->OnCharDestroyed();
	}
	Super::Destroyed();
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
	DOREPLIFETIME(ATpsCharacter, bAlive);
}

float ATpsCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser)
{
	if (!Data) return 0;
	
	if (DamageAmount > 0) DamageAmount /= GetArmorRatio();
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (HasAuthority() && !FMath::IsNearlyZero(DamageAmount))
	{
		const auto HealAmount = FMath::Min(-DamageAmount, Data->MaxHP - HP);
		if (HealAmount >= 2 && EventInstigator && EventInstigator != GetController())
			if (const auto PS = EventInstigator->GetPlayerState<ASaucewichPlayerState>())
				PS->AddScore(TEXT("Heal"), FMath::Min(-DamageAmount, Data->MaxHP - HP) / 2);

		HP = FMath::Clamp(HP - DamageAmount, 0.f, Data->MaxHP);
		if (FMath::IsNearlyZero(HP)) Kill(EventInstigator ? EventInstigator->GetPlayerState<ASaucewichPlayerState>() : nullptr, DamageCauser);
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

	if (const auto GS = GetWorld()->GetGameState<ASaucewichGameState>())
		if (!GS->ShouldPlayerTakeDamage(this, DamageAmount, DamageEvent, EventInstigator, DamageCauser))
			return false;

	if (!EventInstigator)
		return true;

	if (const auto InstigatorState = EventInstigator->GetPlayerState<ASaucewichPlayerState>())
		return DamageAmount > 0 ? GetTeam() != InstigatorState->GetTeam() : GetTeam() == InstigatorState->GetTeam();

	return true;
}

void ATpsCharacter::SetPlayerDefaults()
{
	SetActorActivated(true);

	if (Data != nullptr)
	{
		if (HasAuthority())
		{
			HP = Data->MaxHP;
			bAlive = true;
		}
		if (Data->RespawnInvincibleTime > 0)
		{
			BeTranslucent();
			GetWorldTimerManager().SetTimer(
				RespawnInvincibleTimer, 
				this, &ATpsCharacter::BeOpaque,
				Data->RespawnInvincibleTime
			);
		}
	}

	if (const auto PC = GetController<ASaucewichPlayerController>())
	{
		PC->OnCharRespawn.Broadcast();
	}
	
	OnCharacterSpawn.Broadcast();
}

float ATpsCharacter::GetArmorRatio_Implementation() const
{
	return WeaponComponent->GetArmorRatio();
}

void ATpsCharacter::Kill(ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	if (HasAuthority())
		MulticastKill(Attacker, Inflictor);
}

void ATpsCharacter::KillSilent()
{
	bAlive = false;
	SetActorActivated(false);

	if (HasAuthority())
	{
		const auto MyPS = GetPlayerState();
		UE_LOG(LogCharacter, Log, TEXT("%s was killed silently"), MyPS ? *MyPS->GetPlayerName() : *GetName());
	}

	WeaponComponent->OnCharacterDeath();
	OnCharacterDeath.Broadcast();

	if (const auto PC = GetController<ASaucewichPlayerController>())
	{
		PC->OnCharDied.Broadcast();
	}
}

void ATpsCharacter::MoveForward(const float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue);
}

void ATpsCharacter::MoveRight(const float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue);
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
	SetColor(GetTeamColor());
}

void ATpsCharacter::BindOnTeamChanged()
{
	if (const auto PS = GetPlayerState())
	{
		const auto Player = CastChecked<ASaucewichPlayerState>(PS);
		Player->OnTeamChangedDelegate.AddDynamic(this, &ATpsCharacter::OnTeamChanged);
		OnTeamChanged(Player->GetTeam());
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ATpsCharacter::BindOnTeamChanged);
	}
}

int32 ATpsCharacter::GetColIdx() const
{
	return GetMesh()->GetMaterialIndex(Data->ColMatName);
}

void ATpsCharacter::OnRep_Alive()
{
	if (bAlive) SetPlayerDefaults();
	else KillSilent();
}

void ATpsCharacter::MulticastKill_Implementation(ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	Kill_Internal(Attacker, Inflictor);
}

void ATpsCharacter::Kill_Internal(ASaucewichPlayerState* const Attacker, AActor* const Inflictor)
{
	HP = 0;
	bAlive = false;
	SetActorActivated(false);

	WeaponComponent->OnCharacterDeath();
	OnCharacterDeath.Broadcast();
	
	if (const auto PC = GetController<ASaucewichPlayerController>())
	{
		PC->OnCharDied.Broadcast();
	}
	
	if (HasAuthority())
	{
		if (const auto GameMode = GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
			if (const auto PC = GetController<ASaucewichPlayerController>())
				GameMode->SetPlayerRespawnTimer(PC);

		const auto MyPS = GetPlayerState();
		
		if (const auto GameState = GetWorld()->GetGameState<ASaucewichGameState>())
			GameState->MulticastPlayerDeath(Cast<ASaucewichPlayerState>(MyPS), Attacker, Inflictor);

		UE_LOG(LogCharacter, Log, TEXT("%s was killed by %s with %s"),
			MyPS ? *MyPS->GetPlayerName() : *GetName(),
			Attacker ? *Attacker->GetPlayerName() : TEXT("unknown"),
			Inflictor ? *Inflictor->GetName() : TEXT("unknown")
		);
	}

	OnKilled();
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

	FTimerDelegate Delegate;
	Delegate.BindWeakLambda(this, [this, PerkClass]
	{
		if (const auto Found = Perks.Find(PerkClass))
			if (Found->PSC) Found->PSC->ReleaseToPool();
		
		Perks.Remove(PerkClass);
	});
	GetWorldTimerManager().SetTimer(Perk.Timer, Delegate, Def->GetDuration(), false);
}
