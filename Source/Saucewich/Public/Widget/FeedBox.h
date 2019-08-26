// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UFeedBox final : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	void MakeNewFeed(const struct FFeedContent& NewFeedContent);

private:	
	UPROPERTY(Transient)
	TArray<class UFeed*> Feeds;

	UPROPERTY(Transient)
	class UVerticalBox* FeedBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TSubclassOf<UFeed> FeedClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	uint8 FeedNum;
	
	uint8 CurFeedNum;
};
