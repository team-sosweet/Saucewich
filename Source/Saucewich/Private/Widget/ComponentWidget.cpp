// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Widget/ComponentWidget.h"

#include "Widget/BaseWidget.h"

void UComponentWidget::Init(UBaseWidget* InOwnerWidget)
{
	OwnerWidget = InOwnerWidget;
}
