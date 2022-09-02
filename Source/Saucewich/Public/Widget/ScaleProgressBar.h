// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ScaleProgressBar.generated.h"

UCLASS()
class SAUCEWICH_API UScaleProgressBar : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintSetter)
	void SetValue(float InValue);

private:
	FAnchors GetAnchorByValue() const;
	
	UPROPERTY(Transient)
	class UImage* LeftImage;

	UPROPERTY(Transient)
	UImage* RightImage;

	UPROPERTY(Transient)
	class UProgressBar* ProgressBar;
	
	UPROPERTY(Transient)
	class UCanvasPanelSlot* RightSlot;
	
	UPROPERTY(EditInstanceOnly, BlueprintSetter = "SetValue", meta = (AllowPrivateAccess = true))
	float Value;
	
	FAnchors StartAnchor;

	float ProgressMinAnchor;

	float ProgressMaxAnchor;
};
