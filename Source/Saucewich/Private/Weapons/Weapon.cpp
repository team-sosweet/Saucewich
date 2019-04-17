// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "UnrealNetwork.h"
#include "SauceProjectile.h"
#include "ActorPoolComponent.h"
#include "SaucewichCharacter.h"

AWeapon::AWeapon()
	:SceneRoot{ CreateDefaultSubobject<USceneComponent>("SceneRoot") },
	Mesh{ CreateDefaultSubobject<UStaticMeshComponent>("Mesh") },
	Muzzle{ CreateDefaultSubobject<USceneComponent>("Muzzle") },
	ProjectilePool{ CreateDefaultSubobject<UActorPoolComponent>("ProjectilePool") }
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = SceneRoot;
	Mesh->SetupAttachment(SceneRoot);
	Muzzle->SetupAttachment(Mesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	ASaucewichCharacter* const Character = GetInstigator<ASaucewichCharacter>();
	if (Character)
	{
		Role = Character->Role;
		CopyRemoteRoleFrom(Character);
		Character->OnDeath.AddUObject(this, &AWeapon::SetActivated, false);
		SauceAmount = GetData().SauceAmount;
	}
}

void AWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttacking && CanAttack())
	{
		HandleAttack();
	}
	Reload(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, SauceAmount);
	DOREPLIFETIME(AWeapon, bAttacking);
}

void AWeapon::SetActivated(bool bActive)
{
	SetActorTickEnabled(bActive);
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
}

const FWeaponData& AWeapon::GetData() const
{
	if (!DataTable)
	{
		DataTable = DataTableHandle.GetRow<FWeaponData>(TEXT(""));
	}
	return *DataTable;
}

//////////////////////////////////////////////////////////////////////////////

#define ENSURE_NOT_SIMULATED_PROXY() ensureMsgf(Role != ROLE_SimulatedProxy, TEXT(__FUNCTION__)TEXT(" called on simulated proxy. It won't have any effect!"))

void AWeapon::Attack()
{
	if (ENSURE_NOT_SIMULATED_PROXY() && !bAttacking)
	{
		if (GetData().bFullAuto)
		{
			bAttacking = true;
			bOldAttacking = true;
		}

		if (CanAttack())
		{
			HandleAttack();
		}

		if (Role != ROLE_Authority)
		{
			ServerAttack();
		}
	}
}

void AWeapon::StopAttack()
{
	if (ENSURE_NOT_SIMULATED_PROXY() && bAttacking)
	{
		bAttacking = false;
		bOldAttacking = false;

		if (Role != ROLE_Authority)
		{
			ServerStopAttack();
		}
	}
}

bool AWeapon::CanAttack() const
{
	ASaucewichCharacter* const Character = GetInstigator<ASaucewichCharacter>();
	return Character && Character->Alive() && SauceAmount > 0 && NextAttackTime <= GetWorld()->GetTimeSeconds() && !bDried;
}

void AWeapon::OnRep_Attacking()
{
	if (bAttacking && !bOldAttacking)
	{
		HandleAttack();
	}
	bOldAttacking = bAttacking;
}

void AWeapon::HandleAttack()
{
	AActor* const Sauce = ShootSauce();
	if (Sauce)
	{
		NextAttackTime = GetWorld()->GetTimeSeconds() + GetData().AttackDelay;

		if (Role != ROLE_SimulatedProxy)
		{
			LastSauceAmount = --SauceAmount;
			bDried = SauceAmount <= 0;
			ReloadAlpha = 0.f;
			ReloadWaitTime = 0.f;
		}
	}
}

AActor* AWeapon::ShootSauce()
{
	APawn* const Pawn = GetInstigator();
	if (!Pawn) return nullptr;

	auto& Data = GetData();

	FTransform SpawnTransform = Muzzle->GetComponentTransform();

	const FVector Start = Pawn->GetPawnViewLocation();
	const FVector End = Start + Pawn->GetBaseAimRotation().Vector() * Data.ProjectileSpeed;
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByProfile(Hit, Start, End, "Projectile"))
	{
		SpawnTransform.SetRotation((Hit.Location - SpawnTransform.GetLocation()).ToOrientationQuat());
	}
	
	FActorSpawnParameters Param;
	Param.Instigator = Pawn;
	Param.Owner = this;
	bool bReused;
	ASauceProjectile* const Sauce = ProjectilePool->SpawnActor<ASauceProjectile>(SpawnTransform, Param, &bReused);
	if (Sauce)
	{
		Sauce->Init(Data.Damage, Data.ProjectileSpeed);
		if (!bReused)
		{
			Pawn->MoveIgnoreActorAdd(Sauce);
		}
	}
	return Sauce;
}

void AWeapon::ServerAttack_Implementation() { Attack(); if (!GetData().bFullAuto) MulticastSingleAttack(); }
bool AWeapon::ServerAttack_Validate() { return true; }
void AWeapon::ServerStopAttack_Implementation() { StopAttack(); }
bool AWeapon::ServerStopAttack_Validate() { return true; }
void AWeapon::MulticastSingleAttack_Implementation() { if (Role == ROLE_SimulatedProxy) HandleAttack(); }

void AWeapon::Reload(const float DeltaTime)
{
	if (Role != ROLE_Authority) return;
	auto& Data = GetData();
	if (SauceAmount < Data.SauceAmount)
	{
		if (ReloadWaitTime >= Data.ReloadWaitTime)
		{
			ReloadAlpha = FMath::Clamp(ReloadAlpha + DeltaTime / Data.ReloadTime, 0.f, 1.f);
			SauceAmount = FMath::CubicInterp<float>(LastSauceAmount, 0.f, Data.SauceAmount, 0.f, ReloadAlpha);
			if (bDried && SauceAmount >= Data.MinSauceAmountToShootWhenFullReload)
			{
				bDried = false;
			}
		}
		else
		{
			ReloadWaitTime += DeltaTime;
		}
	}
}
