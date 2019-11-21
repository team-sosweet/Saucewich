// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ErrorWidget.generated.h"

UCLASS()
class SAUCEWICH_API UErrorWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	void Activate(const FText& Message, bool bCritical);

private:
	UFUNCTION()
	void OnClicked();
	
	UPROPERTY(Transient)
	class UTextBlock* DescriptionText;

	UPROPERTY(Transient)
	class UButton* BackgroundButton;

	uint8 bCriticalError : 1;
};
