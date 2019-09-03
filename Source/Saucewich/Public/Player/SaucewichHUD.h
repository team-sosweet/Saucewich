// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "SaucewichHUD.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;

private:
	UFUNCTION()
	void OnSpawn();

	UFUNCTION()
	void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UAliveHUD> AliveWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UDeathHUD> DeathWidgetClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UAliveHUD* AliveWidget;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDeathHUD* DeathWidget;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float DeathWidgetDelay;

	FTimerHandle DeathWidgetTimer;
};
