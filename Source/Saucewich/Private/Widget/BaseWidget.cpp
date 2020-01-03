// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/BaseWidget.h"
#include "GameFramework/InputSettings.h"
#include "Names.h"
#include "Player/BaseHUD.h"
#include "Saucewich.h"
#include "Player/SaucewichPlayerController.h"

void UBaseWidget::ShowError(const FText Message) const
{
	HUD->ShowError(Message);
}

void UBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const auto PC = GetOwningPlayer();
	HUD = CastChecked<ABaseHUD>(PC->GetHUD());

	if (bVisibleOnlyAlive)
	{
		if (const auto SwPC = Cast<ASaucewichPlayerController>(PC))
		{
			SwPC->OnPlyRespawnNative.AddUObject(this, &UWidget::SetVisibility, ESlateVisibility::HitTestInvisible);
			SwPC->OnPlyDeathNative.AddUObject(this, &UWidget::SetVisibility, ESlateVisibility::Collapsed);
		}
	}
}

void UBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (bIsFocusable)
	{
		HUD->AddFocusedWidget(this);
		SetFocus();
	}

	OnConstruct.Broadcast();
}

void UBaseWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (IsValid(HUD) && bIsFocusable) HUD->RemoveFocusedWidget(this);
	OnDestruct.Broadcast();
}

FReply UBaseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsCloseable)
	{
		if (USaucewich::CheckInputAction(NAME("Close"), InKeyEvent))
		{
			RemoveFromParent();
		}
	}
	else if (USaucewich::CheckInputAction(NAME("Menu"), InKeyEvent))
	{
		HUD->OpenMenu();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
