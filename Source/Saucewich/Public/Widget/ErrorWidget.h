// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "BaseWidget.h"
#include "ErrorWidget.generated.h"

UCLASS()
class SAUCEWICH_API UErrorWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UErrorWidget();
	void Activate(FText&& Message);

protected:
	void NativeOnInitialized() override;
	
private:
	class UTextBlock* Text;
};
