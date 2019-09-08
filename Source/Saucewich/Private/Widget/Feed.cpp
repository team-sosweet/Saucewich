// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/Feed.h"

#include "TimerManager.h"

void UFeed::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UFeed::ViewFeed(const float LifeTime)
{
	CurLifeTime = LifeTime;
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, [this]
		{
			CurLifeTime -= 0.1f;

			if (CurLifeTime <= 0.0f)
			{
				GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
				SetVisibility(ESlateVisibility::Hidden);
				OnExpiration.ExecuteIfBound();
			}
		}, 0.1f, true);
}