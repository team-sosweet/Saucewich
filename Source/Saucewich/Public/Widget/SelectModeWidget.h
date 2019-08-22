// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "ModeWidget.h"
#include "SelectModeWidget.generated.h"

UCLASS()
class SAUCEWICH_API USelectModeWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	class UModeWidget* CreateModeWidget(uint8 Index);

private:	
	void OnModeSelect(uint8 Index);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mode, Meta = (AllowPrivateAccess = true))
	TArray<FMode> Modes;

	UPROPERTY(Transient)
	TArray<class UModeWidget*> ModeWidgets;

	UPROPERTY(Transient)
	class UHorizontalBox* ModeBox;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Mode, Meta = (AllowPrivateAccess = true))
	uint8 SelectIndex;
};
