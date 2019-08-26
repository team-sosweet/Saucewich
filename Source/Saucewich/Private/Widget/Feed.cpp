// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Feed.h"

#include "TimerManager.h"

void UFeed::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UFeed::ViewFeed()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, [this]
		{
			SetVisibility(ESlateVisibility::Hidden);
			OnExpiration.ExecuteIfBound();
		}, LifeTime, false);
}