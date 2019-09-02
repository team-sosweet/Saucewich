// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/LobbyWidget.h"

#include "Components/Button.h"

void ULobbyWidget::CreateMenuWidget(const TSubclassOf<UUserWidget> MenuClass, UButton* Button)
{
	const auto Menu = CreateWidget(GetOwningPlayer(), MenuClass);
	ButtonWidgetMap.Add(Button, Menu);
	
	Button->OnClicked.AddUnique(this, &ULobbyWidget)
}
