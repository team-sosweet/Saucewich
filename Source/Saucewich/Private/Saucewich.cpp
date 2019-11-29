// Copyright 2019 Othereum. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"

#include "GameLiftServerSDK.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Saucewich, "Saucewich")

DEFINE_LOG_CATEGORY(LogGameLift)
DEFINE_LOG_CATEGORY(LogSaucewich)

FGameLiftServerSDKModule& USaucewich::GetGameLift()
{
	static auto& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>(TEXT("GameLiftServerSDK"));
	return Module;
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
