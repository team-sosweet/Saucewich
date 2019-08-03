// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SaucewichHUD.generated.h"

/**
 * 
 */
UCLASS()
class SAUCEWICH_API ASaucewichHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHUD(class UBaseHUD*& AliveHUD, class UDeathHUD*& DeathHUD);

private:
	UFUNCTION()
	void OnSpawn();

	UFUNCTION()
	void OnDeath();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float DeathWidgetDelay;

	FTimerHandle DeathWidgetTimer;

	UBaseHUD* AliveWidget;
	UDeathHUD* DeathWidget;
};
