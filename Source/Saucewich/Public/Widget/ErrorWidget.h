// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "BaseWidget.h"
#include "ErrorWidget.generated.h"

UCLASS()
class SAUCEWICH_API UErrorWidget : public UBaseWidget
{
	GENERATED_BODY()

public:
	UErrorWidget();
	void Activate(const FText& Message, bool bCritical);

protected:
	void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void OnClicked();
	
	UPROPERTY(Transient)
	class UTextBlock* DescriptionText;

	UPROPERTY(Transient)
	class UButton* BackgroundButton;

	uint8 bCriticalError : 1;
};
