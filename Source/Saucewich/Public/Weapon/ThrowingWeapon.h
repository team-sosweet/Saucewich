// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Weapon/Weapon.h"
#include "ThrowingWeapon.generated.h"

USTRUCT(BlueprintType)
struct FThrowingWeaponData : public FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ThrowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadTime;
};

/*
 * 던지는 무기입니다.
 * 주의: 이것은 무기 그 자체를 나타내고, 실제 던져지는 것은 projectile 입니다.
 */
UCLASS(Abstract)
class SAUCEWICH_API AThrowingWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	const FThrowingWeaponData& GetThrowingWeaponData() const;

	UFUNCTION(BlueprintCallable)
	float GetRemainingReloadTime() const;

protected:
	void SlotP() override;
	void OnReleased() override;

private:
	FTimerHandle ReloadTimer;
};
