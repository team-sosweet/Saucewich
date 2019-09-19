// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/DedicatedServerDefaultGameMode.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameMode.h"
#include "SaucewichGameInstance.h"

void ADedicatedServerDefaultGameMode::BeginPlay()
{
	auto& GameModes = GetGameInstance<USaucewichGameInstance>()->GetGameModes();
	const auto GmClass = GameModes[FMath::RandHelper(GameModes.Num())];
	const auto DefGm = GmClass.GetDefaultObject();

	auto& AvailableMaps = DefGm->GetAvailableMaps();
	const auto NewMap = AvailableMaps[FMath::RandHelper(AvailableMaps.Num())].GetAssetName();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap, *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}
