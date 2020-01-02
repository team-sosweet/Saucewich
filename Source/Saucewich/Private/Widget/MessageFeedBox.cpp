// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/MessageFeedBox.h"
#include "Widget/MessageFeed.h"

void UMessageFeedBox::CopyContent(UFeed* const Dest, const UFeed* const Src)
{
	FMessageFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
