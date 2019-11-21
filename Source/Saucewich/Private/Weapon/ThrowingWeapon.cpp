// Copyright 2019 Othereum. All Rights Reserved.

#include "ThrowingWeapon.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "Entity/ActorPool.h"
#include "Weapon/Projectile/Projectile.h"

const FThrowingWeaponData& AThrowingWeapon::GetThrowingWeaponData() const
{
	return GetData<FThrowingWeaponData>();
}

float AThrowingWeapon::GetRemainingReloadTime() const
{
	return FMath::Max(GetWorldTimerManager().GetTimerRemaining(ReloadTimer), 0.f);
}

void AThrowingWeapon::SlotP()
{
	auto& TimerManager = GetWorldTimerManager();
	if (TimerManager.TimerExists(ReloadTimer)) return;

	auto&& Data = GetThrowingWeaponData();
	if (!ensure(Data.ProjectileClass)) return;

	if (HasAuthority() || !Data.ProjectileClass.GetDefaultObject()->GetIsReplicated())
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.Instigator = GetInstigator();

		if (const auto Thrown = AActorPool::Get(this)->Spawn<AProjectile>(Data.ProjectileClass, Data.ThrowOffset * GetActorTransform(), Parameters))
		{
			Thrown->ResetSpeed();
		}
	}
	
	TimerManager.SetTimer(ReloadTimer, Data.ReloadTime, false);
}

void AThrowingWeapon::OnReleased()
{
	Super::OnReleased();
	GetWorldTimerManager().ClearTimer(ReloadTimer);
}
