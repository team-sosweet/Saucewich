// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Weapon/Weapon.h"
#include "ThrowingWeapon.generated.h"

UCLASS(Abstract)
class AThrowingWeapon final : public AWeapon
{
	GENERATED_BODY()

	void SlotP() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

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
