// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Widget/Feed.h"

#include "TimerManager.h"

void UFeed::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetVisibility(ESlateVisibility::Hidden);
}

void UFeed::ViewFeed(const float LifeTime)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, this, &UFeed::LifetimeExpired, LifeTime);
}

float UFeed::GetCurLifeTime() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(LifeTimerHandle);
}

void UFeed::LifetimeExpired()
{
	SetVisibility(ESlateVisibility::Hidden);
	OnExpiration.ExecuteIfBound();
}
