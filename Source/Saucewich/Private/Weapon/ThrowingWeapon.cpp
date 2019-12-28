// Copyright 2019 Othereum. All Rights Reserved.

#include "Weapon/ThrowingWeapon.h"

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
	const auto ProjCls = Data.ProjectileClass.LoadSynchronous();
	check(ProjCls);

	if (HasAuthority() || !GetDefault<AActor>(ProjCls)->GetIsReplicated())
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.Instigator = GetInstigator();

		if (const auto Thrown = AActorPool::Get(this)->Spawn<AProjectile>(ProjCls, Data.ThrowOffset * GetActorTransform(), Parameters))
		{
			Thrown->ResetSpeed();
		}
	}

	auto&& Callback = FTimerDelegate::CreateUObject(this, &AWeapon::OnAvailabilityChanged, true);
	TimerManager.SetTimer(ReloadTimer, MoveTemp(Callback), Data.ReloadTime, false);
	OnAvailabilityChanged(false);
}

void AThrowingWeapon::OnReleased()
{
	Super::OnReleased();
	GetWorldTimerManager().ClearTimer(ReloadTimer);
}
