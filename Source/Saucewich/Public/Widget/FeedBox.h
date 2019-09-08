// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FeedBox.generated.h"

UCLASS()
class SAUCEWICH_API UFeedBox : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;
	
public:
	void MakeNewFeed(const struct FFeedContent& NewFeedContent);

protected:
	virtual void CopyContent(class UFeed* Dest, UFeed* Src);
	
private:	
	UPROPERTY(Transient)
	TArray<UFeed*> Feeds;

	UPROPERTY(Transient)
	class UVerticalBox* FeedBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TSubclassOf<UFeed> FeedClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	uint8 FeedNum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setting, meta = (AllowPrivateAccess = true))
	float FeedLifeTime;
	
	uint8 CurFeedNum;
};
