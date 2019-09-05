// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/BaseWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Widget/ComponentWidget.h"

void UBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	RootPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root")));
}

void UBaseWidget::SetComponent(const TSubclassOf<UComponentWidget> ComponentClass)
{
	if (!ComponentClass)
		return;
	
	if (Component)
	{
		Component->RemoveFromParent();
	}

	Component = CreateWidget<UComponentWidget>(GetOwningPlayer(), ComponentClass);
	Component->AddToViewport();
	
	const auto ComponentSlot = RootPanel->AddChildToCanvas(Component);
	ComponentSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	ComponentSlot->SetPosition(FVector2D(0.0f, 0.0f));
	ComponentSlot->SetSize(FVector2D(0.0f, 0.0f));
}
