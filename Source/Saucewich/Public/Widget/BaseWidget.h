// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

UCLASS()
class SAUCEWICH_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;
	void NativeDestruct() override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
