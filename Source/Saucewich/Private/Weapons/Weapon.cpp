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
	}
}

void AWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttacking && CanAttack())
	{
		HandleAttack();
	}
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

//////////////////////////////////////////////////////////////////////////////

#define ENSURE_NOT_SIMULATED_PROXY() ensureMsgf(Role != ROLE_SimulatedProxy, TEXT(__FUNCTION__)TEXT(" called on simulated proxy. It won't have any effect!"))

void AWeapon::Attack()
{
	if (ENSURE_NOT_SIMULATED_PROXY() && !bAttacking)
	{
		if (bFullAuto)
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
	return Character && Character->CanAttack() && SauceAmount > 0 && NextAttackTime <= GetWorld()->GetTimeSeconds();
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
	if (Sauce && Role != ROLE_SimulatedProxy)
	{
		--SauceAmount;
	}
	NextAttackTime = GetWorld()->GetTimeSeconds() + AttackDelay;
}

AActor* AWeapon::ShootSauce()
{
	FTransform SpawnTransform = Muzzle->GetComponentTransform();

	const FVector Start = GetInstigator()->GetPawnViewLocation();
	const FVector End = Start + GetInstigator()->GetBaseAimRotation().Vector() * 10000.f;
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByProfile(Hit, Start, End, "Projectile"))
	{
		SpawnTransform.SetRotation((Hit.Location - SpawnTransform.GetLocation()).ToOrientationQuat());
	}

	FActorSpawnParameters Param;
	Param.Instigator = GetInstigator();
	Param.Owner = this;
	ASauceProjectile* const Sauce = ProjectilePool->SpawnActor<ASauceProjectile>(SpawnTransform, Param);
	if (Sauce)
	{
		Sauce->Init(Damage, MuzzleSpeed);
	}
	return Sauce;
}

void AWeapon::ServerAttack_Implementation() { Attack(); if (!bFullAuto) MulticastSingleAttack(); }
bool AWeapon::ServerAttack_Validate() { return true; }
void AWeapon::ServerStopAttack_Implementation() { StopAttack(); }
bool AWeapon::ServerStopAttack_Validate() { return true; }
void AWeapon::MulticastSingleAttack_Implementation() { if (Role == ROLE_SimulatedProxy) HandleAttack(); }
