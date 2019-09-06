// Copyright 2019 Team Sosweet. All Rights Reserved.

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
class SAUCEWICH_API AThrowingWeapon final : public AWeapon
{
	GENERATED_BODY()

public:
	// 무기 데이터에 대한 레퍼런스를 반환합니다.
	// 만약 무기 클래스에 데이터가 바인드 되어있지 않거나 하는 이유로 데이터를 구할 수 없을 경우 기본값을 반환합니다.
	UFUNCTION(BlueprintCallable)
	const FThrowingWeaponData& GetThrowingWeaponData() const;

protected:
	void SlotP() override;
	void OnActivated() override;
	void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ReloadingTime;

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bReloading : 1;
};
