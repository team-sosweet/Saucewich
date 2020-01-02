// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "FridgeHUD.generated.h"

UCLASS()
class SAUCEWICH_API UFridgeHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(uint8 InFridgeTeam);

	UFUNCTION(BlueprintImplementableEvent)
	void SetHighlighted(bool bHighlight);

private:
	void InitWithPS(class ASaucewichPlayerState* InPS);
	void OnPlayerTeamChanged(uint8 NewTeam);
	
	uint8 FridgeTeam;
};
