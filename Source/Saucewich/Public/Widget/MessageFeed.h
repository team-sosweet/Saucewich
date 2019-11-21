// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Widget/Feed.h"
#include "MessageFeed.generated.h"

USTRUCT(BlueprintType)
struct FMessageFeedContent : public FFeedContent
{
	GENERATED_BODY()

	FMessageFeedContent() = default;
	FMessageFeedContent(const FText& Text);

	UPROPERTY(BlueprintReadOnly)
	FText Text;
};

UCLASS()
class SAUCEWICH_API UMessageFeed : public UFeed
{
	GENERATED_BODY()
	
public:
	void GetContent(FFeedContent& OutContent) const override;
	void SetContent(const FFeedContent& InContent) override;

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FMessageFeedContent Content;
};
