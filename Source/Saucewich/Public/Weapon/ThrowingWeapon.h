// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Weapon.h"
#include "ThrowingWeapon.generated.h"

/*
 * 던지는 무기입니다.
 * 이것은 가상의 무기를 나타내고, 실제 던져지는 것은 projectile 입니다.
 */
UCLASS(Abstract)
class AThrowingWeapon final : public AWeapon
{
	GENERATED_BODY()

	void SlotP() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FTransform ThrowOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<class AProjectile> ProjectileClass;
	class AActorPool* ProjectilePool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ReloadTime;

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ReloadingTime;

	UPROPERTY(Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bReloading : 1;
};
