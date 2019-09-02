// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class SAUCEWICH_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Menu Widget"))
	void CreateMenuWidget(TSubclassOf<UUserWidget> MenuClass, class UButton* Button);

private:
	void OnOpenMenu();
	
	UPROPERTY(Transient)
	TMap<UButton*, UUserWidget*> ButtonWidgetMap;
};
