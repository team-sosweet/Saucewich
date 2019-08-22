// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "SaucewichHUD.generated.h"

UCLASS()
class SAUCEWICH_API ASaucewichHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

private:
	UFUNCTION()
	void OnSpawn();

	UFUNCTION()
	void OnDeath();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float DeathWidgetDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UAliveHUD> AliveWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UDeathHUD> DeathWidgetClass;

	FTimerHandle DeathWidgetTimer;

	UAliveHUD* AliveWidget;
	UDeathHUD* DeathWidget;
};
