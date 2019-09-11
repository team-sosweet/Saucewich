// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngredientWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(TArray<uint8>, FGetIngredientNums);

UCLASS()
class SAUCEWICH_API UIngredientWidget final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	TArray<uint8> GetIngredientNums();

private:
	void SetIngredientImage(uint8 IngredientIndex, uint8 Num);
	
	UPROPERTY(Transient, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<class UCanvasPanel*> IngredientPanels;

	UPROPERTY(Transient)
	TArray<TSubclassOf<UUserWidget>> IngredientContentClasses;

	UPROPERTY(Transient)
	TArray<class UCanvasPanelSlot*> IngredientContentSlots;
	
	UPROPERTY(Transient)
	TArray<class UUniformGridSlot*> IngredientsSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool IsRightDirection;
	
	TArray<bool> IsIngredientActives;

	uint8 IngredientsNum;
	
	uint8 ActiveNum;
	
	int8 DirectionSign;
};