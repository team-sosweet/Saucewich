// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "MessageFeed.h"

FMessageFeedContent::FMessageFeedContent(const FText& Text)
	:Text{Text}
{
}

void UMessageFeed::GetContent(FFeedContent& OutContent) const
{
	static_cast<FMessageFeedContent&>(OutContent) = Content;
}

void UMessageFeed::SetContent(const FFeedContent& InContent)
{
	Content = static_cast<const FMessageFeedContent&>(InContent);
	Super::SetContent(InContent);
}
