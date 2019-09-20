// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameInstance.h"

#include "Engine/World.h"

#include "Entity/ActorPool.h"
#include "GameMode/SaucewichGameState.h"
#include "JsonData.h"

AActorPool* USaucewichGameInstance::GetActorPool()
{
	if (!IsValid(ActorPool)) ActorPool = GetWorld()->SpawnActor<AActorPool>();
	return ActorPool;
}

ASaucewichGameState* USaucewichGameInstance::GetGameState() const
{
	return GetWorld()->GetGameState<ASaucewichGameState>();
}

float USaucewichGameInstance::GetSensitivity() const
{
	return CorrectionValue * Sensitivity + CorrectionValue * .5f;
}

void USaucewichGameInstance::BeginDestroy()
{
	Super::BeginDestroy();

	FJson Json;
	Json.Data.Add(TEXT("port"), UJsonData::MakeStringData(FString::FromInt(GetWorld()->URL.Port)));
	PostRequest(TEXT("crash"), Json, {});
}
