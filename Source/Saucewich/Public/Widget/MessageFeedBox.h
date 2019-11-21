// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Widget/FeedBox.h"
#include "MessageFeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UMessageFeedBox : public UFeedBox
{
	GENERATED_BODY()
	
protected:
	void CopyContent(UFeed* Dest, const UFeed* Src) override;
};
