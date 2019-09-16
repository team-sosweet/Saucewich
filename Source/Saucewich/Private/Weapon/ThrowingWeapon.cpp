// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "ThrowingWeapon.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "Entity/ActorPool.h"
#include "Weapon/Projectile/Projectile.h"

const FThrowingWeaponData& AThrowingWeapon::GetThrowingWeaponData() const
{
	static const FThrowingWeaponData Default{};
	const auto Data = GetData<FThrowingWeaponData>(TEXT("AThrowingWeapon::GetThrowingWeaponData()"));
	return Data ? *Data : Default;
}

float AThrowingWeapon::GetRemainingReloadTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(ReloadTimer), 0.f);
}

void AThrowingWeapon::SlotP()
{
	auto& TimerManager = GetWorldTimerManager();
	if (TimerManager.TimerExists(ReloadTimer)) return;

	const auto Data = GetData<FThrowingWeaponData>(TEXT("AThrowingWeapon::SlotP()"));
	if (!Data || !Data->ProjectileClass) return;

	if (HasAuthority() || !Data->ProjectileClass.GetDefaultObject()->GetIsReplicated())
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.Instigator = GetInstigator();

		if (const auto Thrown = GetPool()->Spawn<AProjectile>(Data->ProjectileClass, Data->ThrowOffset * GetActorTransform(), Parameters))
		{
			Thrown->ResetSpeed();
			Thrown->SetColor(GetColor());
		}
	}
	
	TimerManager.SetTimer(ReloadTimer, Data->ReloadTime, false);
}
