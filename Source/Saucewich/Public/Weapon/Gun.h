// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Gun.generated.h"

UCLASS(Abstract)
class AGun : public AWeapon
{
	GENERATED_BODY()

public:
	void FireP() override;
	void FireR() override;
	void SlotP() override;
	
protected:
	void Tick(float DeltaSeconds) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent)
	void Shoot();

private:
	UPROPERTY(EditAnywhere)
	float Rpm;

	float FireLag;
	float LastFire;

	UPROPERTY(EditAnywhere)
	uint8 ClipSize;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Clip;

	UPROPERTY(Replicated, Transient, EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bFiring : 1;
};
