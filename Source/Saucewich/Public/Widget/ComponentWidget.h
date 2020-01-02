// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "BaseWidget.h"
#include "ComponentWidget.generated.h"

UCLASS()
class SAUCEWICH_API UComponentWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	void Init(class UCompositeWidget* InOwnerWidget);
	
private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCompositeWidget* OwnerWidget;
};
