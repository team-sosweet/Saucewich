// Copyright 2019 Othereum. All Rights Reserved.

#include "SaucewichInstance.h"
#include "Engine/World.h"
#include "Entity/ActorPool.h"
#include "Entity/SauceMarker.h"
#include "UserSettings.h"
#include "Engine/Engine.h"
#include "Saucewich.h"
#include "GameMode/DSDefGM.h"

#if WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
	#include "Misc/OutputDeviceFile.h"
	#include "HAL/PlatformOutputDevices.h"
#endif

template <class T>
static T* GetOrSpawn(T*& Ptr, UClass* const Class, UWorld* const World)
{
	if (!IsValid(Ptr)) Ptr = World->SpawnActor<T>(Class);
	return Ptr;
}

template <class T>
static T* GetOrSpawn(T*& Ptr, UWorld* const World)
{
	return GetOrSpawn(Ptr, T::StaticClass(), World);
}

USaucewichInstance* USaucewichInstance::Get(const UObject* const WorldContextObj)
{
	return WorldContextObj->GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
}

AActorPool* USaucewichInstance::GetActorPool() const
{
	return GetOrSpawn(ActorPool, GetWorld());
}

ASauceMarker* USaucewichInstance::GetSauceMarker() const
{
	return GetOrSpawn(SauceMarker, SauceMarkerClass.LoadSynchronous(), GetWorld());
}

bool USaucewichInstance::PopNetworkError(TEnumAsByte<ENetworkFailure::Type>& Type, FString& Msg)
{
	if (!LastNetworkError.bOccured) return false;
	
	Msg = MoveTemp(LastNetworkError.Msg);
	Type = LastNetworkError.Type;
	
	LastNetworkError.bOccured = false;
	return true;
}

#if WITH_GAMELIFT
static void Terminate()
{
	UE_LOG(LogGameLift, Log, TEXT("OnTerminated called. Terminating process..."));
	GameLift::SafeTerminate();
}

static bool CheckHealth()
{
	UE_LOG(LogGameLift, Verbose, TEXT("OnHealthCheck called"));
	return true;
}

namespace GameLift
{
	extern void Check(const FGameLiftGenericOutcome& Outcome);
}

void USaucewichInstance::StartGameSession(Aws::GameLift::Server::Model::GameSession) const
{
	UE_LOG(LogGameLift, Log, TEXT("OnStartGameSession called. Starting game..."));

	const auto GameMode = GetWorld()->GetAuthGameMode();
	if (const auto DsGm = Cast<ADSDefGM>(GameMode))
	{
		DsGm->BeginStartGame();
	}
	else
	{
		UE_LOG(LogGameLift, Error, TEXT("Unable to start the game. The current game mode must be DSDefGM, but it is %s. Terminating the process..."), *GameMode->GetClass()->GetName());
		GameLift::SafeTerminate();
	}
}
#endif

void USaucewichInstance::Init()
{
	Super::Init();

	UserSettings = NewObject<UUserSettings>();
	UserSettings->CommitMaxFPS();

	GEngine->NetworkFailureEvent.AddUObject(this, &USaucewichInstance::OnNetworkError);

#if WITH_GAMELIFT
	UE_LOG(LogGameLift, Log, TEXT("Starting GameLift SDK..."));

	auto& Module = GameLift::Get();
	GameLift::Check(Module.InitSDK());
	
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

	static FProcessParameters Params;
	Params.OnStartGameSession.BindUObject(this, &USaucewichInstance::StartGameSession);
	Params.OnTerminate.BindStatic(Terminate);
	Params.OnHealthCheck.BindStatic(CheckHealth);
	
	Params.port = GetWorld()->URL.Port;
	UE_LOG(LogGameLift, Log, TEXT("Port: %d"), Params.port);

	Params.logParameters.Add(FPlatformOutputDevices::GetAbsoluteLogFilename());
	UE_LOG(LogGameLift, Log, TEXT("Log file: %s"), *Params.logParameters[0]);

	GameLift::Check(Module.ProcessReady(Params));
	UE_LOG(LogGameLift, Log, TEXT("Ready for create game session"));
#endif
}

void USaucewichInstance::OnNetworkError(UWorld*, UNetDriver*, const ENetworkFailure::Type Type, const FString& Msg)
{
	LastNetworkError.Msg = Msg;
	LastNetworkError.Type = Type;
	LastNetworkError.bOccured = true;
}
