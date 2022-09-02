// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "SaucewichInstance.h"

#include "Engine/World.h"
#include "Engine/Engine.h"

#include "Entity/ActorPool.h"
#include "Entity/SauceMarker.h"
#include "UserSettings.h"
#include "Matchmaker.h"
#include "Saucewich.h"
#include "GameMode/SaucewichGameMode.h"

#if defined(WITH_GAMELIFT) && WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
	#include "Misc/OutputDeviceFile.h"
	#include "HAL/PlatformOutputDevices.h"
	#include "GameMode/DSDefGM.h"
#endif

#define LOCTEXT_NAMESPACE ""

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

USaucewichInstance::~USaucewichInstance()
{
#if defined(WITH_GAMELIFT) && WITH_GAMELIFT
	if (GameSession)
	{
		delete GameSession;
		GameSession = nullptr;
	}
#endif
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

bool USaucewichInstance::PopNetworkError(FText& OutMsg)
{
	if (!LastNetworkError.bOccured) return false;
	
	OutMsg = LastNetworkError.Msg;
	LastNetworkError.bOccured = false;
	return true;
}

void USaucewichInstance::PushNetworkError(const FText& Msg)
{
	LastNetworkError.Msg = Msg;
	LastNetworkError.bOccured = true;
}

void USaucewichInstance::Init()
{
	Super::Init();

	UserSettings = NewObject<UUserSettings>(this);
	Matchmaker = NewObject<UMatchmaker>(this);

	GEngine->NetworkFailureEvent.AddUObject(this, &USaucewichInstance::OnNetworkError);

	UE_LOG(LogSaucewich, Log, TEXT("BUILD TIME: " __DATE__ " " __TIME__));
}

void USaucewichInstance::OnNetworkError(UWorld*, UNetDriver*, const ENetworkFailure::Type Type, const FString& Msg)
{
	const auto EnumPtr = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("ENetworkFailure"), true);
	PushNetworkError(FMT_MSG(LOCTEXT("OnNetworkError", "{0}\n{1}"),
		FText::FromString(EnumPtr->GetNameStringByIndex(Type)), FText::FromString(Msg)));
}


#if defined(WITH_GAMELIFT) && WITH_GAMELIFT

namespace GameLift
{
	extern void Check(const FGameLiftGenericOutcome& Outcome);
}

const Aws::GameLift::Server::Model::GameSession& USaucewichInstance::GetGameSession() const
{
	check(GameSession);
	return *GameSession;
}

void USaucewichInstance::StartGameSession(Aws::GameLift::Server::Model::GameSession&& Session)
{
	UE_LOG(LogGameLift, Log, TEXT("OnStartGameSession called. Starting game..."));
	*GameSession = MoveTemp(Session);

	const auto GameMode = GetWorld()->GetAuthGameMode();
	if (const auto DsGm = Cast<ADSDefGM>(GameMode))
	{
		bShouldActivateGameSession = true;
		DsGm->BeginStartGame();
	}
	else
	{
		UE_LOG(LogGameLift, Error, TEXT("Unable to start the game. The current game mode must be DSDefGM, but it is %s. Terminating the process..."), *GameMode->GetClass()->GetName());
		GameLift::SafeTerminate();
	}
}

void USaucewichInstance::UpdateGameSession(Aws::GameLift::Server::Model::UpdateGameSession&& Updated)
{
	using Aws::GameLift::Server::Model::UpdateReasonMapper::GetNameForUpdateReason;
	const auto ReasonStr = UTF8_TO_TCHAR(GetNameForUpdateReason(Updated.GetUpdateReason()));
	UE_LOG(LogGameLift, Log, TEXT("UpdateGameSession called. Reason: %s"), ReasonStr);
	
	if (Updated.GetUpdateReason() == Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED)
	{
		*GameSession = Updated.GetGameSession();
	}
}

void OnStartGameSession(Aws::GameLift::Server::Model::GameSession GameSession, void* State)
{
	const auto GI = static_cast<USaucewichInstance*>(State);
    GI->StartGameSession(MoveTemp(GameSession));
}

void OnUpdateGameSession(Aws::GameLift::Server::Model::UpdateGameSession Updated, void* State)
{
	const auto GI = static_cast<USaucewichInstance*>(State);
    GI->UpdateGameSession(MoveTemp(Updated));
}

void OnProcessTerminate(void* State)
{
	const auto GI = static_cast<USaucewichInstance*>(State);
	if (const auto Gm = GI->GetWorld()->GetAuthGameMode<ASaucewichGameMode>())
	{
		Gm->OnProcessTerminate();
	}
	else
	{
		UE_LOG(LogGameLift, Log, TEXT("OnTerminated called. Terminating process..."));
		GameLift::SafeTerminate();
	}
}

bool OnHealthCheck(void* State)
{
	UE_LOG(LogGameLift, Verbose, TEXT("OnHealthCheck called"));
	return true;
}

#endif

void USaucewichInstance::StartupServer()
{
#if defined(WITH_GAMELIFT) && WITH_GAMELIFT
	if (!bIsGameLiftInitialized)
	{
		UE_LOG(LogGameLift, Log, TEXT("Starting GameLift SDK..."));
		auto& Module = GameLift::Get();
		GameLift::Check(Module.InitSDK());
		UE_LOG(LogGameLift, Log, TEXT("GameLift SDK Initialized"));

		GameSession = new Aws::GameLift::Server::Model::GameSession;
		
		const auto Port = GetWorld()->URL.Port;
		UE_LOG(LogGameLift, Log, TEXT("Port: %d"), Port);

		const auto LogFile = static_cast<FOutputDeviceFile*>(FPlatformOutputDevices::GetLog())->GetFilename();
		const char* LogFileUTF8 = TCHAR_TO_UTF8(LogFile);
		UE_LOG(LogGameLift, Log, TEXT("Log file: %s"), LogFile);

		const Aws::GameLift::Server::ProcessParameters Params
		{
	    	OnStartGameSession, this,
	    	OnUpdateGameSession, this,
	    	OnProcessTerminate, this,
	    	OnHealthCheck, this,
	    	Port, {&LogFileUTF8, 1}
	    };

		const auto Outcome = ProcessReady(Params);
		if (Outcome.IsSuccess())
		{
			UE_LOG(LogGameLift, Log, TEXT("Ready for create game session"));
			bIsGameLiftInitialized = true;
		}
		else
		{
			auto&& Err = Outcome.GetError();
			UE_LOG(LogGameLift, Error, TEXT("%s: %s"), UTF8_TO_TCHAR(Err.GetErrorName()), UTF8_TO_TCHAR(Err.GetErrorMessage()));
			UE_LOG(LogGameLift, Error, TEXT("Terminating..."));
			GameLift::SafeTerminate();
		}
	}
#endif
}

void USaucewichInstance::OnGameReady()
{
#if defined(WITH_GAMELIFT) && WITH_GAMELIFT
	if (bShouldActivateGameSession)
	{
		UE_LOG(LogGameLift, Log, TEXT("Activating game session..."));
		GameLift::Check(GameLift::Get().ActivateGameSession());
		UE_LOG(LogGameLift, Log, TEXT("Game session activated."));
		bShouldActivateGameSession = false;
	}
#endif
}

#undef LOCTEXT_NAMESPACE
