// Copyright (c) 2019, Team Sosweet. All rights reserved.

#include "Gun.h"
#include "UnrealNetwork.h"
#include "ActorPoolComponent.h"
#include "SauceProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

AGun::AGun()
	:Muzzle{ CreateDefaultSubobject<USceneComponent>("Muzzle") },
	ProjectilePool{ CreateDefaultSubobject<UActorPoolComponent>("ProjectilePool") }
{
	Muzzle->SetupAttachment(RootComponent, "Muzzle");
}

void AGun::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttacking && CanAttack())
	{
		HandleAttack();
	}
	Reload(DeltaTime);
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, SauceAmount);
	DOREPLIFETIME(AGun, bAttacking);
}

#define ENSURE_NOT_SIMULATED_PROXY() ensureMsgf(Role != ROLE_SimulatedProxy, TEXT(__FUNCTION__)TEXT(" called on simulated proxy. It won't have any effect!"))

void AGun::StartAttack()
{
	if (ENSURE_NOT_SIMULATED_PROXY() && !bAttacking)
	{
		if (GetData()->bFullAuto)
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

void AGun::StopAttack()
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

bool AGun::CanAttack() const
{
	return Super::CanAttack() && SauceAmount > 0 && NextAttackTime <= GetWorld()->GetTimeSeconds() && !bDried;
}

void AGun::Equip(const FWeaponData* NewWeaponData)
{
	Super::Equip(NewWeaponData);

	const auto Data{ GetData() };
	SauceAmount = Data->SauceAmount;
	ProjectilePool->SetDefaultActorClass(Data->ProjectileClass);
}

void AGun::OnRep_Attacking()
{
	if (bAttacking && !bOldAttacking)
	{
		HandleAttack();
	}
	bOldAttacking = bAttacking;
}

void AGun::HandleAttack()
{
	if (ShootSauce())
	{
		NextAttackTime = GetWorld()->GetTimeSeconds() + GetData()->AttackDelay;

		if (Role != ROLE_SimulatedProxy)
		{
			LastSauceAmount = --SauceAmount;
			bDried = SauceAmount <= 0;
			ReloadAlpha = 0.f;
			ReloadWaitTime = 0.f;
		}
	}
}

AActor* AGun::ShootSauce()
{
	const auto Pawn{ GetInstigator() };
	if (!Pawn) return nullptr;

	const auto Data{ GetData() };

	auto SpawnTransform{ Muzzle->GetComponentTransform() };

	const auto Start{ Pawn->GetPawnViewLocation() };
	const auto End{ Start + Pawn->GetBaseAimRotation().Vector() * Data->ProjectileSpeed };
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByProfile(Hit, Start, End, "Projectile"))
	{
		SpawnTransform.SetRotation((Hit.Location - SpawnTransform.GetLocation()).ToOrientationQuat());
	}
	SpawnTransform.SetScale3D(FVector{ Data->ProjectileScale });

	FActorSpawnParameters Param;
	Param.Instigator = Pawn;
	Param.Owner = this;
	bool bReused;
	const auto Sauce{ ProjectilePool->SpawnActor<ASauceProjectile>(SpawnTransform, Param, &bReused) };
	if (Sauce)
	{
		Sauce->SetSpeed(Data->ProjectileSpeed);
		if (!bReused)
		{
			// 현재 기획상 게임 플레이 도중 대미지나 색상이 바뀔 일이 없어서 최초 스폰시에만 설정함
			Sauce->Damage = Data->Damage;
			Sauce->SetColor(GetColor());
			Pawn->MoveIgnoreActorAdd(Sauce);
		}
	}
	return Sauce;
}

void AGun::ServerAttack_Implementation() { StartAttack(); if (!GetData()->bFullAuto) MulticastSingleAttack(); }
bool AGun::ServerAttack_Validate() { return true; }
void AGun::ServerStopAttack_Implementation() { StopAttack(); }
bool AGun::ServerStopAttack_Validate() { return true; }
void AGun::MulticastSingleAttack_Implementation() { if (Role == ROLE_SimulatedProxy) HandleAttack(); }

void AGun::Reload(const float DeltaTime)
{
	if (Role != ROLE_Authority) return;
	const auto Data{ GetData() };
	if (SauceAmount < Data->SauceAmount)
	{
		if (ReloadWaitTime >= Data->ReloadWaitTime)
		{
			ReloadAlpha = FMath::Clamp(ReloadAlpha + DeltaTime / Data->ReloadTime, 0.f, 1.f);
			SauceAmount = FMath::CubicInterp<float>(LastSauceAmount, 0.f, Data->SauceAmount, 0.f, ReloadAlpha);
			if (bDried && SauceAmount >= Data->MinSauceAmountToShootWhenFullReload)
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

UClass* FGunData::GetBaseClass() const
{
	return AGun::StaticClass();
}
