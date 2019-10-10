// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/DedicatedServerDefaultGameMode.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameMode.h"
#include "SaucewichGameInstance.h"
#include "JsonData.h"

void ADedicatedServerDefaultGameMode::BeginPlay()
{
#if !WITH_EDITOR
	const auto GI = GetGameInstance<UHttpGameInstance>();
	const auto Port = GetWorld()->URL.Port;
	const auto MaxPort = GI->GetMaxPort();
	
	if (Port <= MaxPort)
	{
		GI->PortForServer = GetWorld()->URL.Port;

		FJson Json;
		Json.Data.Add(TEXT("port"), UJsonData::MakeStringData(FString::FromInt(Port)));
		
		FOnResponded OnResponded;
		OnResponded.BindDynamic(this, &ADedicatedServerDefaultGameMode::OnServerRegistered);
		
		GI->PostRequest(TEXT("room/port"), Json, OnResponded);
		UE_LOG(LogExternalServer, Log, TEXT("Requesting server registration with port %d..."), Port);
	}
	else
	{
		UE_LOG(LogExternalServer, Error, TEXT("Port is out of maximum range! (Current: %d, Max: %d) Unable to register server."), Port, MaxPort);
		StartServer();
	}
#else
	StartServer();
#endif
}

void ADedicatedServerDefaultGameMode::OnServerRegistered(const bool bIsSuccess, const int32 Code, FJson Json)
{
	if (bIsSuccess)
	{
		UE_LOG(LogExternalServer, Log, TEXT("Server registration successful"));
	}
	else
	{
		UE_LOG(LogExternalServer, Error, TEXT("Failed to register server! Error code: %d"), Code);
	}
	StartServer();
}

void ADedicatedServerDefaultGameMode::StartServer() const
{
	auto& GameModes = GetGameInstance<USaucewichGameInstance>()->GetGameModes();
	const auto GmClass = GameModes[FMath::RandHelper(GameModes.Num())];
	const auto DefGm = GmClass.GetDefaultObject();

	auto& AvailableMaps = DefGm->GetAvailableMaps();
	const auto NewMap = AvailableMaps[FMath::RandHelper(AvailableMaps.Num())].GetAssetName();

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap, *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}
