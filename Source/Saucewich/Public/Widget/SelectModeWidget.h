// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModeWidget.h"
#include "SelectModeWidget.generated.h"

UCLASS()
class SAUCEWICH_API USelectModeWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeOnInitialized() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	class UModeWidget* CreateModeWidget(uint8 Index);

private:
	void OnModeSelect(uint8 Index);

protected:
	UPROPERTY()
	class UHorizontalBox* ModeBox;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mode, Meta = (AllowPrivateAccess = true))
	TArray<FMode> Modes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mode, Meta = (AllowPrivateAccess = true))
	uint8 SelectIndex;

	TArray<class UModeWidget*> ModeWidgets;
};
