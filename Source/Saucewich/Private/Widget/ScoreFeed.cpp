// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ScoreFeed.h"

void UScoreFeed::GetContent(FFeedContent& OutContent) const
{
	auto& Content = static_cast<FScoreFeedContent&>(OutContent);

	Content.DisplayName = DisplayName;
	Content.Score = Score;
}

void UScoreFeed::SetContent(const FFeedContent& InContent)
{
	const auto& Content = static_cast<const FScoreFeedContent&>(InContent);

	DisplayName = Content.DisplayName;
	Score = Content.Score;

	OnSetContent();
}