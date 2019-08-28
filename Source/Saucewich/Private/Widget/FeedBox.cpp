// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/FeedBox.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "Widget/Feed.h"

void UFeedBox::NativeOnInitialized()
{
	FeedBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("Box")));
	
	for (uint8 Num = 0; Num < FeedNum; Num++)
	{
		const auto Feed = CreateWidget<UFeed>(GetOwningPlayer(), FeedClass);
		Feed->OnExpiration.BindLambda([this]
			{
				CurFeedNum--;
			});
		
		const auto FeedSlot = FeedBox->AddChildToVerticalBox(Feed);
		FeedSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Feeds.Add(Feed);
	}

	const auto CollapseFeed = CreateWidget<UFeed>(GetOwningPlayer(), FeedClass);
	CollapseFeed->SetVisibility(ESlateVisibility::Collapsed);
	Feeds.Add(CollapseFeed);
}

void UFeedBox::MakeNewFeed(const FFeedContent& NewFeedContent)
{
	for (auto Index = CurFeedNum; Index > 0u; Index--)
	{
		FFeedContent Content;
		Feeds[Index - 1]->GetContent(Content);
		Feeds[Index]->SetContent(Content);
	}
	
	Feeds[0]->SetContent(NewFeedContent);

	if (CurFeedNum < FeedNum)
	{
		Feeds[CurFeedNum++]->ViewFeed();
	}
}