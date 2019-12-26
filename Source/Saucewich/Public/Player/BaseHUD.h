// Copyright 2019 Othereum. All Rights Reserved.

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
	void ShowError(FText Message, bool bCritical);

	UFUNCTION(BlueprintCallable)
	void OpenMenu() const;

	void AddFocusedWidget(class UWidget* Widget);
	void RemoveFocusedWidget(UWidget* Widget);

protected:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowError();

private:
	UPROPERTY(Transient, VisibleInstanceOnly)
	TArray<TWeakObjectPtr<UWidget>> FocusedWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<class UErrorWidget> ErrorWidgetClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class UBaseWidget> MenuWidgetClass;

	UPROPERTY(Transient)
	UErrorWidget* ErrorWidget;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UBaseWidget* MenuWidget;
};
