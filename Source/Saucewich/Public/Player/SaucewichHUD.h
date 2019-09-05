// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SaucewichHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeColor, FLinearColor, MyTeamColor);

UCLASS()
class SAUCEWICH_API ASaucewichHUD final : public AHUD
{
	GENERATED_BODY()

	void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnChangeColor OnChangeColor;

private:
	UFUNCTION()
	void ChangeColor(uint8 NewTeam);
	
	void BindChangeColor();

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UAliveWidget* AliveWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UDeathWidget* DeathWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UResultWidget* ResultWidget;

	class ASaucewichGameState* GameState;
};
