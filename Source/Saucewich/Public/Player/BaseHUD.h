// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "UObject/TextProperty.h"
#include "BaseHUD.generated.h"

UCLASS()
class SAUCEWICH_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	class UErrorWidget* ShowError(FText Message);

	UFUNCTION(BlueprintCallable)
	void OpenMenu() const;

	void AddFocusedWidget(class UWidget* Widget);
	void RemoveFocusedWidget(UWidget* Widget);

protected:
	void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<class UBaseWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UErrorWidget> ErrorWidgetClass;

	UPROPERTY(Transient, VisibleInstanceOnly)
	TArray<TWeakObjectPtr<UWidget>> FocusedWidgets;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBaseWidget* MenuWidget;
	
	UPROPERTY(Transient)
	UErrorWidget* ErrorWidget;
};
