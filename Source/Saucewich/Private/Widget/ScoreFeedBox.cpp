// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Widget/ScoreFeedBox.h"

#include "Widget/ScoreFeed.h"

void UScoreFeedBox::CopyContent(UFeed* Dest, const UFeed* Src)
{
	FScoreFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
