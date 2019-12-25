// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ComponentWidget.generated.h"

UCLASS()
class SAUCEWICH_API UComponentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(class UCompositeWidget* InOwnerWidget);
	
private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCompositeWidget* OwnerWidget;
};
