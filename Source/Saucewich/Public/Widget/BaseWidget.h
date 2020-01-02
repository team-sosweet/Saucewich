// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConstruct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestruct);

UCLASS()
class SAUCEWICH_API UBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Interaction", BlueprintReadWrite)
	uint8 bIsCloseable : 1;

	UPROPERTY(EditAnywhere, Category="Behavior", BlueprintReadWrite)
	uint8 bVisibleOnlyAlive : 1;

protected:
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(BlueprintAssignable)
	FOnConstruct OnConstruct;

	UPROPERTY(BlueprintAssignable)
	FOnDestruct OnDestruct;
};
