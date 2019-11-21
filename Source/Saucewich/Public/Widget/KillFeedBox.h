// Copyright 2019 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Widget/FeedBox.h"
#include "KillFeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UKillFeedBox : public UFeedBox
{
	GENERATED_BODY()

	void CopyContent(UFeed* Dest, const UFeed* Src) override;
};
