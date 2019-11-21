// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "Widget/KillFeedBox.h"

#include "Widget/KillFeed.h"

void UKillFeedBox::CopyContent(UFeed* Dest, const UFeed* Src)
{
	FKillFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
