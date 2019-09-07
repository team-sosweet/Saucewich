// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Widget/Feed.h"
#include "ScoreFeed.generated.h"

struct FScoreFeedContent final : FFeedContent
{
	FScoreFeedContent(const FName InName, const int32 InScore)
		: Name(InName), Score(InScore) {}

	FName Name;
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
	FText Name;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	int32 Score;
};
