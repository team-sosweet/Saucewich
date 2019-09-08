// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Widget/FeedBox.h"
#include "ScoreFeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UScoreFeedBox final : public UFeedBox
{
	GENERATED_BODY()

	void CopyContent(UFeed* Dest, UFeed* Src) override;
};
