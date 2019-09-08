// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/ScoreFeedBox.h"

#include "Widget/ScoreFeed.h"

void UScoreFeedBox::CopyContent(UFeed* Dest, UFeed* Src)
{
	FScoreFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
