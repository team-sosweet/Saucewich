// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "MessageFeedBox.h"
#include "Widget/MessageFeed.h"

void UMessageFeedBox::CopyContent(UFeed* const Dest, const UFeed* const Src)
{
	FMessageFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
