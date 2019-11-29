// Copyright 2019 Othereum. All Rights Reserved.

#include "GameMode/DedicatedServerDefaultGameMode.h"

#include "Engine/World.h"
#include "OutputDeviceFile.h"
#include "PlatformOutputDevices.h"
#include "GameLiftServerSDK.h"

#include "SaucewichGameMode.h"
#include "SaucewichInstance.h"

void ADedicatedServerDefaultGameMode::BeginPlay()
{
#if WITH_GAMELIFT || 1

	const auto Check = [](const FGameLiftGenericOutcome& Result)
	{
		auto& Error = Result.GetError();
		checkf(Result.IsSuccess(), TEXT("ERROR: %s: %s"), *Error.m_errorName, *Error.m_errorMessage);
	};
	
	UE_LOG(LogGameLift, Log, TEXT("Starting GameLift SDK..."));

	auto& GameLiftSdkModule = USaucewich::GetGameLift();
	Check(GameLiftSdkModule.InitSDK());
	
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

	static FProcessParameters Params;
	Params.OnStartGameSession.BindWeakLambda(this, 
		[&](const Aws::GameLift::Server::Model::GameSession&)
		{
			StartServer();
			GameLiftSdkModule.ActivateGameSession();
		}
	);

	Params.OnTerminate.BindLambda([&]
	{
		GameLiftSdkModule.ProcessEnding();
		FPlatformMisc::RequestExit(false);
	});
	
	Params.OnHealthCheck.BindLambda([]{return true;});
	
	Params.port = GetWorld()->URL.Port;
	UE_LOG(LogGameLift, Log, TEXT("Port: %d"), Params.port);

	const auto LogFile = static_cast<FOutputDeviceFile*>(FPlatformOutputDevices::GetLog());
	Params.logParameters.Emplace(LogFile->GetFilename());

	Check(GameLiftSdkModule.ProcessReady(Params));
	UE_LOG(LogGameLift, Log, TEXT("Ready for create game session"));

#else

	StartServer();

#endif
}

void ADedicatedServerDefaultGameMode::StartServer() const
{
	auto&& GameModes = USaucewichInstance::Get(this)->GetGameModes();
	const auto GmClass = GameModes[FMath::RandHelper(GameModes.Num())];
	const auto DefGm = GetDefault<ASaucewichGameMode>(GmClass.LoadSynchronous());

	auto&& Maps = DefGm->GetData().Maps;
	const auto NewMap = Maps[FMath::RandHelper(Maps.Num())];

	const auto URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?listen"), *NewMap.GetAssetName(), *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
}
