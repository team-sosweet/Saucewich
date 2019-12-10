// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "TutorialManager.generated.h"

USTRUCT()
struct FTutorial
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint8 MaxCount = 2;

	UPROPERTY()
	uint8 Count;
};

UCLASS()
class SAUCEWICH_API UTutorialManager : public USaveGame
{
	GENERATED_BODY()
};
