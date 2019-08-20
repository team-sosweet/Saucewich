// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "ModeWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnClick, uint8)

USTRUCT(Atomic, BlueprintType)
struct FMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText Description;
};

UCLASS()
class SAUCEWICH_API UModeWidget : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

public:
	void SetSelect(bool bIsSelect);

private:
	UFUNCTION()
	void OnClicked();

public:
	FOnClick OnClick;

protected:
	UPROPERTY()
	class UButton* ModeButton;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mode, Meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FMode Mode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mode, Meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	uint8 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mode, Meta = (AllowPrivateAccess = true))
	bool bIsSelected;
};
