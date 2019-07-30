// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SaucewichPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawned, class ASaucewichPlayerState*, PlayerState);

UCLASS()
class SAUCEWICH_API ASaucewichPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateSpawned OnPlayerStateSpawned;
};
