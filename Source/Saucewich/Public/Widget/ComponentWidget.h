// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ComponentWidget.generated.h"

UCLASS()
class SAUCEWICH_API UComponentWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(class UBaseWidget* InOwnerWidget);
	
private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBaseWidget* OwnerWidget;
};
