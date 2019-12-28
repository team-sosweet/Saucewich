// Copyright 2019 Othereum. All Rights Reserved.

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
	TSoftClassPtr<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadTime;
};

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
