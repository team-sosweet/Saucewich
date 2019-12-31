// Copyright 2019 Othereum. All Rights Reserved.

#include "SaucewichInstance.h"
#include "Engine/World.h"
#include "Entity/ActorPool.h"
#include "Entity/SauceMarker.h"
#include "UserSettings.h"
#include "Engine/Engine.h"

#if WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
	#include "Misc/OutputDeviceFile.h"
	#include "HAL/PlatformOutputDevices.h"
#endif

#include "Saucewich.h"
#include "GameMode/SaucewichGameMode.h"

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

void USaucewichInstance::StartGame() const
{
	const auto GmClass = GameModes[FMath::RandHelper(GameModes.Num())];
	const auto DefGm = GetDefault<ASaucewichGameMode>(GmClass.LoadSynchronous());

	auto&& Maps = DefGm->GetData().Maps;
	auto&& NewMap = Maps[FMath::RandHelper(Maps.Num())];

	const auto URL = FString::Printf(TEXT("%s?game=%s?listen"), *NewMap.GetAssetName(), *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL);
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
	GameLift::Get().ProcessEnding();
	FPlatformMisc::RequestExit(false);
}

static bool CheckHealth()
{
	UE_LOG(LogGameLift, Verbose, TEXT("OnHealthCheck called: Healthy"));
	return true;
}

static void Check(const FGameLiftGenericOutcome& Outcome)
{
	if (!Outcome.IsSuccess())
	{
		auto&& Error = Outcome.GetError();
		UE_LOG(LogGameLift, Error, TEXT("FATAL ERROR: [%s] %s"), *Error.m_errorName, *Error.m_errorMessage);
		UE_LOG(LogGameLift, Error, TEXT("Terminating process..."));
		FPlatformMisc::RequestExit(false);
	}
}

void USaucewichInstance::StartGameSession(Aws::GameLift::Server::Model::GameSession) const
{
	StartGame();
	Check(GameLift::Get().ActivateGameSession());
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
	Check(Module.InitSDK());
	
	UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

	static FProcessParameters Params;
	Params.OnStartGameSession.BindUObject(this, &USaucewichInstance::StartGameSession);
	Params.OnTerminate.BindStatic(Terminate);
	Params.OnHealthCheck.BindStatic(CheckHealth);
	
	Params.port = GetWorld()->URL.Port;
	UE_LOG(LogGameLift, Log, TEXT("Port: %d"), Params.port);

	Params.logParameters.Add(FPlatformOutputDevices::GetAbsoluteLogFilename());
	UE_LOG(LogGameLift, Log, TEXT("Absolute log file name: %s"), *Params.logParameters[0]);

	Check(Module.ProcessReady(Params));
	UE_LOG(LogGameLift, Log, TEXT("Ready for create game session"));
#endif
}

void USaucewichInstance::OnNetworkError(UWorld*, UNetDriver*, const ENetworkFailure::Type Type, const FString& Msg)
{
	LastNetworkError.Msg = Msg;
	LastNetworkError.Type = Type;
	LastNetworkError.bOccured = true;
}
