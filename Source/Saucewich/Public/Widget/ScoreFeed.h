// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Widget/Feed.h"
#include "ScoreFeed.generated.h"

struct FScoreFeedContent final : FFeedContent
{
	FScoreFeedContent() = default;
	
	FScoreFeedContent(const FText& InDisplayName, const int32 InScore)
		: DisplayName(InDisplayName), Score(InScore) {}

	FText DisplayName;
	int32 Score;
};

UCLASS()
class SAUCEWICH_API UScoreFeed final : public UFeed
{
	GENERATED_BODY()
	
public:
	void GetContent(FFeedContent& OutContent) const override;
	void SetContent(const FFeedContent& InContent) override;

private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	FText DisplayName;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	int32 Score;
};
