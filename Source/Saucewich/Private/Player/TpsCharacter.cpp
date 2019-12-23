// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"

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
#include "Names.h"
#include "Entity/SauceMarker.h"
#include "UserSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Log, All)

ATpsCharacter::ATpsCharacter(const FObjectInitializer& ObjectInitializer)
	:Super{ObjectInitializer.SetDefaultSubobjectClass<UTpsCharacterMovementComponent>(CharacterMovementComponentName)},
	WeaponComponent{CreateDefaultSubobject<UWeaponComponent>(Names::WeaponComponent)},
	SpringArm{CreateDefaultSubobject<USpringArmComponent>(Names::SpringArm)},
	Camera{CreateDefaultSubobject<UCameraComponent>(Names::Camera)},
	Shadow{CreateDefaultSubobject<UShadowComponent>(Names::Shadow)}
{
	WeaponComponent->SetupAttachment(GetMesh(), Names::Weapon);
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
	ColMat->GetVectorParameterValue(Names::Color, Color);
	return Color;
}

const FLinearColor& ATpsCharacter::GetTeamColor() const
{
	return ASaucewichGameMode::GetData(this).Teams[GetTeam()].Color;
}

void ATpsCharacter::SetColor(const FLinearColor& NewColor) const
{
	ColMat->SetVectorParameterValue(Names::Color, NewColor);
	ColTranslMat->SetVectorParameterValue(Names::Color, NewColor);
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
	if (GetLocalRole() == ROLE_SimulatedProxy)
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
	OnControllerChanged();
}

void ATpsCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	OnControllerChanged();
}

void ATpsCharacter::OnControllerChanged()
{
	if (const auto Ctrl = GetController())
	{
		const auto PC = CastChecked<ASaucewichPlayerController>(Ctrl);
		ASaucewichPlayerController::BroadcastCharacterSpawned(PC, this);
	}
}

void ATpsCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindOnTeamChanged();

	if (HasAuthority())
	{
		bAlive = true;
		if (Data != nullptr)
		{
			HP = Data->MaxHP;
		}
	}

	const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	GS->AddDilatableActor(this);
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

	Input->BindAxis(NAME("MoveForward"), this, &ATpsCharacter::MoveForward);
	Input->BindAxis(NAME("MoveRight"), this, &ATpsCharacter::MoveRight);
	Input->BindAxis(NAME("Turn"), this, &ATpsCharacter::AddControllerYawInput);
	Input->BindAxis(NAME("LookUp"), this, &ATpsCharacter::AddControllerPitchInput);

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
	check(Data);

	const auto GS = CastChecked<ASaucewichGameState>(GetWorld()->GetGameState());
	if (GS->GetMatchState() == MatchState::Ending) return 0.f;
	
	if (DamageAmount > 0) DamageAmount /= GetArmorRatio();
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (HasAuthority() && !FMath::IsNearlyZero(DamageAmount))
	{
		const auto HealAmount = FMath::Min(-DamageAmount, Data->MaxHP - HP);
		if (HealAmount >= 2 && EventInstigator && EventInstigator != GetController())
			if (const auto PS = EventInstigator->GetPlayerState<ASaucewichPlayerState>())
				PS->AddScore(NAME("Heal"), FMath::Min(-DamageAmount, Data->MaxHP - HP) / 2);

		HP = FMath::Clamp(HP - DamageAmount, 0.f, Data->MaxHP);
		if (FMath::IsNearlyZero(HP)) Kill(EventInstigator ? EventInstigator->GetPlayerState<ASaucewichPlayerState>() : nullptr, DamageCauser);
	}

#if !UE_SERVER
	const auto PC = GetController<APlayerController>();
	if (PC && PC->IsLocalController() && DamageAmount > 0.f)
	{
		const auto Val = FMath::Clamp(DamageAmount / Data->MaxHP, 0.f, 1.f);

		if (UUserSettings::Get()->bVibration)
			PC->PlayDynamicForceFeedback(Val, Val, false, true, false, true);

		PC->ClientPlayCameraShake(Data->HitShake.LoadSynchronous(), Val);
	}
#endif
	
	return DamageAmount;
}

bool ATpsCharacter::ShouldTakeDamage(const float DamageAmount, const FDamageEvent& DamageEvent, AController* const EventInstigator, AActor* const DamageCauser) const
{
	if (!CanBeDamaged()) return false;
	if (!IsAlive()) return false;
	if (IsInvincible()) return false;
	if (FMath::IsNearlyZero(DamageAmount)) return false;

	const auto GS = GetWorld()->GetGameState();
	if (!GS) return false;

	const auto GS2 = CastChecked<AGameState>(GS);
	if (GS2->HasMatchEnded()) return false;

	if (!EventInstigator) return true;

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
		if (Data->RespawnInvincibleTime > 0 && !IsLocallyControlled())
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

	SpawnDeathEffects();
	OnKilled();
}

void ATpsCharacter::SpawnDeathEffects() const
{
#if !UE_SERVER
	const auto World = GetWorld();
	auto Location = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(World, Data->DeathSounds[FMath::RandHelper(Data->DeathSounds.Num())].LoadSynchronous(), Location);

	const auto PSC = UGameplayStatics::SpawnEmitterAtLocation(World, Data->DeathFX.LoadSynchronous(), FTransform{ Location }, true, EPSCPoolMethod::AutoRelease);
	PSC->SetColorParameter(Names::Color, GetColor());

	const auto GS = CastChecked<ASaucewichGameState>(World->GetGameState());
	GS->AddDilatablePSC(PSC);

	Location.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ASauceMarker::Add(this, GetTeam(), Location, Data->DeathSauceMarkScale);

	const auto PC = GetController<APlayerController>();
	if (PC && PC->IsLocalController())
	{
		if (UUserSettings::Get()->bVibration)
			PC->ClientPlayForceFeedback(Data->DeathFBB.LoadSynchronous());

		PC->ClientPlayCameraShake(Data->DeathShake.LoadSynchronous());
	}
#endif
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
