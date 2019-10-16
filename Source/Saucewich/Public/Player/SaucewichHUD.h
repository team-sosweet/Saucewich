// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseHUD.h"
#include "SaucewichHUD.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedColor, const FLinearColor&, MyTeamColor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnChangedColorSingle, const FLinearColor&, MyTeamColor);

UCLASS()
class SAUCEWICH_API ASaucewichHUD : public ABaseHUD
{
	GENERATED_BODY()

	void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void BindChangedColor(const FOnChangedColorSingle& InDelegate);

private:
	UFUNCTION()
	void OnGetPlayerState(class ASaucewichPlayerState* PS);
	
	UFUNCTION()
	void ChangedColor(uint8 NewTeam);

	FOnChangedColor OnChangedColor;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UAliveWidget* AliveWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UDeathWidget* DeathWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = true))
	class UResultWidget* ResultWidget;

	class ASaucewichGameState* GameState;

	FLinearColor MyTeamColor;
};
