// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"

#include "UserSettings.h"
#include "PoolActor.h"

#if WITH_GAMELIFT

	#include "GameLiftServerSDK.h"
	DEFINE_LOG_CATEGORY(LogGameLift)

	FGameLiftServerSDKModule& USaucewich::GetGameLift()
	{
		static auto& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
		return Module;
	}

#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Saucewich, "Saucewich")

DEFINE_LOG_CATEGORY(LogSaucewich)

void USaucewich::CleanupGame(const UObject* WorldContextObject)
{
	check(WorldContextObject);
	const auto World = WorldContextObject->GetWorld();
	check(World);

	for (const auto Actor : TActorRange<APoolActor>{World})
		Actor->Release();
}

ENameValidity USaucewich::IsValidPlayerName(const FString& PlayerName)
{
	if (PlayerName.Len() < GetPlayerNameMinLen() || PlayerName.Len() > GetPlayerNameMaxLen())
		return ENameValidity::Length;

	for (const auto c : PlayerName)
		if (!FChar::IsIdentifier(c))
			return ENameValidity::Character;

	return ENameValidity::Valid;
}

int32 USaucewich::GetPlayerNameMinLen()
{
	return 2;
}

int32 USaucewich::GetPlayerNameMaxLen()
{
	return 16;
}
