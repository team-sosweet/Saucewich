// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "SaucewichGameInstance.generated.h"

UCLASS()
class SAUCEWICH_API USaucewichGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	USaucewichGameInstance();

	class AActorPool* GetActorPool();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<AActorPool> ActorPoolClass;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	AActorPool* ActorPool;
};
