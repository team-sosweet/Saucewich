// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/CompositeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Widget/ComponentWidget.h"

void UCompositeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	RootPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Root")));
}

void UCompositeWidget::SetComponent(const TSubclassOf<UComponentWidget> ComponentClass)
{
	if (!ComponentClass)
		return;
	
	if (Component)
	{
		Component->RemoveFromParent();
	}

	Component = CreateWidget<UComponentWidget>(GetOwningPlayer(), ComponentClass);
	Component->Init(this);
	Component->AddToViewport();
	
	const auto ComponentSlot = RootPanel->AddChildToCanvas(Component);
	ComponentSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	ComponentSlot->SetPosition(FVector2D(0.0f, 0.0f));
	ComponentSlot->SetSize(FVector2D(0.0f, 0.0f));
}
