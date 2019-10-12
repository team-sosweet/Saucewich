// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "MessageFeedBox.h"
#include "Widget/MessageFeed.h"

void UMessageFeedBox::CopyContent(UFeed* const Dest, const UFeed* const Src)
{
	FMessageFeedContent Content;
	Src->GetContent(Content);
	Dest->SetContent(Content);
}
