// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/BlueprintPlatformLibrary.h"

#include "Names.h"

#if WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Saucewich, "Saucewich")

DEFINE_LOG_CATEGORY(LogSaucewich)

#if WITH_GAMELIFT

DEFINE_LOG_CATEGORY(LogGameLift)

namespace GameLift
{
	FGameLiftServerSDKModule& Get()
	{
		static auto&& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>(TEXT("GameLiftServerSDK"));
		return Module;
	}

	void SafeTerminate()
	{
		Get().ProcessEnding();
		FPlatformMisc::RequestExit(false);
	}
	
	void Check(const FGameLiftGenericOutcome& Outcome)
	{
		if (!Outcome.IsSuccess())
		{
			auto&& Error = Outcome.GetError();
			UE_LOG(LogGameLift, Error, TEXT("FATAL ERROR: [%s] %s"), *Error.m_errorName, *Error.m_errorMessage);
			UE_LOG(LogGameLift, Error, TEXT("Terminating process..."));
			SafeTerminate();
		}
	}
}

#endif

bool USaucewich::CheckInputAction(const FName ActionName, const FKeyEvent& KeyEvent)
{
	const auto PressedKey = KeyEvent.GetKey();
	TArray<FInputActionKeyMapping> Mappings;
	GetDefault<UInputSettings>()->GetActionMappingByName(ActionName, Mappings);

	for (auto&& Mapping : Mappings)
		if (Mapping.Key == PressedKey)
			return true;
	
	return false;
}

ENameValidity USaucewich::IsValidPlayerName(const FString& PlayerName)
{
	if (PlayerName.Len() < GetPlayerNameMinLen() || PlayerName.Len() > GetPlayerNameMaxLen())
		return ENameValidity::Length;

	for (const auto c : PlayerName)
	{
#define OUT_OF(a, b) ((TEXT(a) > c) | (c > TEXT(b)))
#define NOT(a) (c!=TEXT(a))
		if (OUT_OF('A', 'Z') & OUT_OF('a', 'z') & OUT_OF('0', '9') & NOT('_') & NOT('-') & NOT('.'))
			return ENameValidity::Character;
#undef NOT
#undef OUT_OF
	}

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

FDateTime USaucewich::ScheduleLocalNotificationAtTime(const int32 Hour, const int32 Minute, const bool bLocalTime, const FText& Title,
	const FText& Body, const FText& Action, const FString& ActivationEvent)
{
	auto Date = bLocalTime ? FDateTime::Now() : FDateTime::UtcNow();

	if (Date.GetHour() > Hour || (Date.GetHour() == Hour && Date.GetMinute() >= Minute))
		Date += FTimespan::FromDays(1);

	int32 Year, Month, Day;
	Date.GetDate(Year, Month, Day);
	const FDateTime Time{Year, Month, Day, Hour, Minute};

	UE_LOG(LogSaucewich, Log, TEXT("Scheduling notification at %s"), *FText::AsDateTime(Time).ToString());

	UBlueprintPlatformLibrary::ScheduleLocalNotificationAtTime(Time, bLocalTime, Title, Body, Action, ActivationEvent);
	return Time;
}
