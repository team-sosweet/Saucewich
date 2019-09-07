// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ScoreFeed.h"

void UScoreFeed::GetContent(FFeedContent& OutContent) const
{
	auto& Content = static_cast<FScoreFeedContent&>(OutContent);

	Content.Name = Name;
	Content.Score = Score;
}

void UScoreFeed::SetContent(const FFeedContent& InContent)
{
	const auto& Content = static_cast<const FScoreFeedContent&>(InContent);

	Name = Content.Name;
	Score = Content.Score;

	OnSetContent();
}