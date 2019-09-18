// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ErrorWidget.generated.h"

UCLASS()
class SAUCEWICH_API UErrorWidget final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	void Activate(const FText& Message);

private:
	UFUNCTION()
	void OnClicked();

	UPROPERTY(Transient)
	class UTextBlock* DescriptionText;

	UPROPERTY(Transient)
	class UButton* BackgroundButton;
};
