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

	if (Port != 0)
	{
		GetRequest(FString::Printf(TEXT("room/crash/%d"), Port), {}, {});
	}
}

void USaucewichGameInstance::RespondGetGameCode(const bool bIsSuccess, const int32 Code, const FJson Json)
{
	if (!bIsSuccess)
	{
		UE_LOG(LogExternalServer, Error, TEXT("Failed to get game code! Error code: %d"), Code);
		return;
	}

	const auto bIsJsonValid = Json.Data.FindRef("roomCode")->AsString(GameCode);
	if (!bIsJsonValid || GameCode.IsEmpty())
	{
		UE_LOG(LogExternalServer, Error, TEXT("Failed to get game code: Invalid Json format"));
		return;
	}

	OnRespondGetGameCode.Broadcast(GameCode);
}
