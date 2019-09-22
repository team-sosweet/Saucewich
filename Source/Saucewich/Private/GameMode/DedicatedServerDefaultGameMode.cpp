// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/DedicatedServerDefaultGameMode.h"

#include "Engine/World.h"

#include "GameMode/SaucewichGameMode.h"
#include "SaucewichGameInstance.h"
#include "JsonData.h"

void ADedicatedServerDefaultGameMode::BeginPlay()
{
	if (const auto GI = GetGameInstance<UHttpGameInstance>())
	{
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

			FOnResponded OnProcReg;
			OnProcReg.BindDynamic(this, &ADedicatedServerDefaultGameMode::OnProcessRegistered);

			const auto ProcID = FPlatformProcess::GetCurrentProcessId();
			GI->PostRequest(FString::Printf(TEXT("game/port/%d"), ProcID), Json, OnProcReg, "localhost:7000/");
			UE_LOG(LogExternalServer, Log, TEXT("Requesting process registration with process ID %d..."), ProcID);
		}
		else
		{
			UE_LOG(LogExternalServer, Error, TEXT("Port is out of maximum range! (Current: %d, Max: %d) Unable to register server."), Port, MaxPort);
			StartServer();
		}
	}
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
	bRegSv = true;
	TryStartServer();
}

void ADedicatedServerDefaultGameMode::OnProcessRegistered(const bool bIsSuccess, const int32 Code, FJson Json)
{
	if (bIsSuccess)
	{
		UE_LOG(LogExternalServer, Log, TEXT("Process registration successful"));
	}
	else
	{
		UE_LOG(LogExternalServer, Error, TEXT("Failed to register process! Error code: %d"), Code);
	}
	bRegProc = true;
	TryStartServer();
}

void ADedicatedServerDefaultGameMode::TryStartServer() const
{
	if (bRegProc && bRegSv) StartServer();
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
