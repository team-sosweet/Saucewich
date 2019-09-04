// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/BaseWidget.h"

#include "Widget/ComponentWidget.h"

void UBaseWidget::SetComponent(const TSubclassOf<UComponentWidget> ComponentClass)
{
	if (Component)
	{
		Component->RemoveFromParent();
	}

	Component = CreateWidget<UComponentWidget>(GetOwningPlayer(), ComponentClass);
}
