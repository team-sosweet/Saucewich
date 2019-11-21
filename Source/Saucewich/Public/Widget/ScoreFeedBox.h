// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Widget/FeedBox.h"
#include "ScoreFeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UScoreFeedBox : public UFeedBox
{
	GENERATED_BODY()

	void CopyContent(UFeed* Dest, const UFeed* Src) override;
};
