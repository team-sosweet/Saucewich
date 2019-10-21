// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "GameMode/DedicatedServerDefaultGameMode.h"

#include "Engine/World.h"
#include "OutputDeviceFile.h"
#include "Paths.h"
#include "PlatformOutputDevices.h"

#include "GameLiftServerSDK.h"

#include "GameMode/SaucewichGameMode.h"
#include "SaucewichGameInstance.h"
#include "SaucewichLibrary.h"
#include "JsonData.h"

void ADedicatedServerDefaultGameMode::BeginPlay()
{
#if WITH_GAMELIFT

	const auto Check = [](const FGameLiftGenericOutcome& Result)
	{
		auto& Error = Result.GetError();
		checkf(Result.IsSuccess(), TEXT("ERROR: %s: %s"), *Error.m_errorName, *Error.m_errorMessage);
	};
	
	auto& GameLiftSdkModule = FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
	Check(GameLiftSdkModule.InitSDK());
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

	FProcessParameters Params;
	Params.OnStartGameSession.BindWeakLambda(
		this, 
		[this, &GameLiftSdkModule](Aws::GameLift::Server::Model::GameSession GameSession)
		{
			GameLiftSdkModule.ActivateGameSession();
			StartServer();
		}
	);

	Params.OnTerminate.BindLambda([&GameLiftSdkModule]{GameLiftSdkModule.ProcessEnding();});
	Params.OnHealthCheck.BindLambda([]{return true;});
	
	Params.port = GetWorld()->URL.Port;
	UE_LOG(LogGameLift, Log, TEXT("Port: %d"), Params.port);

	if (const auto File = static_cast<FOutputDeviceFile*>(FPlatformOutputDevices::GetLog()))
	{
		auto Log = FPaths::ProjectLogDir();
		Log += File->GetFilename();
		Params.logParameters.Add(MoveTemp(Log));
		UE_LOG(LogGameLift, Log, TEXT("Log file: %s"), *Log);
	}

	Check(GameLiftSdkModule.ProcessReady(Params));
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

#else

	StartServer();

#endif
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
