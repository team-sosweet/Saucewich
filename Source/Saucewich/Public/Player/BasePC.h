// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "BasePC.generated.h"

UCLASS()
class SAUCEWICH_API ABasePC : public APlayerController
{
	GENERATED_BODY()

public:
	void AddFocusedWidget(class UWidget* Widget);
	void RemoveFocusedWidget(class UWidget* Widget);

private:
	UPROPERTY(Transient)
	TSet<class UWidget*> FocusedWidgets;
};
