// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "TextProperty.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

UCLASS()
class SAUCEWICH_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void ShowError(FText Message);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UErrorWidget> ErrorWidgetClass;

	UPROPERTY(Transient)
	UErrorWidget* ErrorWidget;
};
