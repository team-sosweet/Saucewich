// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SaucewichHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeColor, FLinearColor, MyTeamColor);

enum class EGameRule : uint8;

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

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TMap<EGameRule, TSubclassOf<class UComponentWidget>> AliveComponentsClass;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TMap<EGameRule, TSubclassOf<UComponentWidget>> DeathComponentsClass;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TMap<EGameRule, TSubclassOf<UComponentWidget>> ResultComponentsClass;
	
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UAliveWidget* AliveWidget;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UDeathWidget* DeathWidget;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UResultWidget* ResultWidget;

	class ASaucewichGameState* GameState;
};
