// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/FeedBox.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "Widget/Feed.h"

void UFeedBox::NativeOnInitialized()
{
	FeedBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("Box")));
	const auto Player = GetOwningPlayer();
	
	for (uint8 Num = 0; Num < FeedNum; Num++)
	{
		const auto Feed = CreateWidget<UFeed>(Player, FeedClass);
		Feed->OnExpiration.BindLambda([this]
			{
				CurFeedNum--;
			});
		
		const auto FeedSlot = FeedBox->AddChildToVerticalBox(Feed);
		FeedSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Feeds.Add(Feed);
	}

	const auto CollapseFeed = CreateWidget<UFeed>(Player, FeedClass);
	CollapseFeed->SetVisibility(ESlateVisibility::Collapsed);
	Feeds.Add(CollapseFeed);
}

void UFeedBox::MakeNewFeed(const FFeedContent& NewFeedContent)
{
	for (auto Index = FMath::Min<int32>(CurFeedNum, FeedNum-1); Index > 0; --Index)
	{
		CopyContent(Feeds[Index], Feeds[Index - 1]);
		Feeds[Index]->ViewFeed(Feeds[Index - 1]->GetCurLifeTime());
	}
	
	Feeds[0]->SetContent(NewFeedContent);
	Feeds[0]->ViewFeed(FeedLifeTime);

	CurFeedNum = FMath::Min<uint8>(CurFeedNum+1, FeedNum);
}

void UFeedBox::CopyContent(UFeed* Dest, const UFeed * Src)
{
	check(0);
}
