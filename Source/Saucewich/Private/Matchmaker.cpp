// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Matchmaker.h"
#include <chrono>
#include "Http.h"
#include "Json.h"
#include "Names.h"
#include "Saucewich.h"
#include "UserSettings.h"
#include "SaucewichInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogMatchmaker, Log, All)

#define LOCTEXT_NAMESPACE ""

namespace Matchmaker
{
	static const FString GBaseURL = TEXT("http://api.saucewich.net");
	
	template <class Fn>
	static bool Request(const FString& Verb, const FString& URL, Fn&& OnResponse)
	{
		const auto Req = FHttpModule::Get().CreateRequest();
		
		Req->SetVerb(Verb);
		Req->SetURL(URL);
		Req->OnProcessRequestComplete().BindLambda(
			[OnResponse=Forward<Fn>(OnResponse)]
			(const FHttpRequestPtr&, const FHttpResponsePtr& Response, bool)
			{
  				const auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				TSharedPtr<FJsonObject> JsonObject;
				FJsonSerializer::Deserialize(Reader, JsonObject);
				static const FJsonObject Default;
				OnResponse(Response->GetResponseCode(), JsonObject ? *JsonObject : Default);
			}
		);

		return Req->ProcessRequest();
	}
}

UMatchmaker::UMatchmaker()
{
	const auto World = UObject::GetWorld();
	if (!World) return;

	UpdatePlayableTime();
	
	const auto Delegate = TBaseDelegate<void>::CreateUObject(this, &UMatchmaker::SetPlayableTimeNotification);
	FCoreDelegates::ApplicationWillDeactivateDelegate.Add(Delegate);
	FCoreDelegates::ApplicationHasReactivatedDelegate.Add(Delegate);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Add(Delegate);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Add(Delegate);
	FCoreDelegates::ApplicationWillTerminateDelegate.Add(Delegate);

	UUserSettings::Get(World)->OnNotificationDisabled.AddWeakLambda(this, [this]
	{
		LastNotificationTime = {};
		SaveConfig();
	});
}

UMatchmaker* UMatchmaker::Get(const UObject* const W)
{
	return USaucewichInstance::Get(W)->GetMatchmaker();
}

void UMatchmaker::StartMatchmaking()
{
	using namespace Matchmaker;
	using namespace std::chrono;

	const auto URL = GBaseURL + TEXT("/session/create?AliasId=alias-53ca2617-e3b1-4c0a-a0e6-a0721b1f8176");
	
	const auto bSuccess = Request(SSTR("GET"), URL, [this](const int32 Code, const FJsonObject& Content)
	{
		if (Code == 200)
		{
			OnMatchmakingComplete(Content);
		}
		else
		{
			const auto Msg = Content.GetStringField(SSTR("description"));
			auto Error = [&](const EMMResponse Response)
			{
				UMatchmaker::Error(Response, *FString::Printf(TEXT("Code: %d, Description: \"%s\""), Code, *Msg));
			};
			
			switch (Code)
			{
			case 500:
				if (Msg.Contains(TEXT("OUTDATED"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
					Error(EMMResponse::Outdated);
				else
					Error(EMMResponse::NotPlayableTime);
				break;
			case 0:
				Error(EMMResponse::ConnFail);
				break;
			default:
				Error(EMMResponse::Error);
			}
		}
		
	});

	if (!bSuccess)
	{
		Error(EMMResponse::ConnFail);
	}
}

void UMatchmaker::BindCallback(const FOnStartMatchmakingResponse& Callback)
{
	OnResponse = Callback;
}

void UMatchmaker::OnMatchmakingComplete(const FJsonObject& Content) const
{
	using namespace Matchmaker;

	const auto Address = Content.GetStringField(SSTR("Address"));
	const auto PlayerID = Content.GetStringField(SSTR("PlayerId"));
	const auto SessionID = Content.GetStringField(SSTR("PlayerSessionId"));

	if (!Address.IsEmpty() && !PlayerID.IsEmpty() && !SessionID.IsEmpty())
	{
		OnResponse.ExecuteIfBound(EMMResponse::OK, Address, PlayerID, SessionID);
	}
	else
	{
		Error(EMMResponse::Error, *FString::Printf(TEXT("Address: %s, PlayerId: %s, PlayerSessionId: %s"), *Address, *PlayerID, *SessionID));
	}
}

void UMatchmaker::Error(const EMMResponse Code, const TCHAR* const Msg) const
{
	const auto EnumPtr = FindObjectChecked<UEnum>(ANY_PACKAGE, TEXT("EMMResponse"), true);
	UE_LOG(LogMatchmaker, Error, TEXT("[%s]: %s"), *EnumPtr->GetNameStringByIndex(int32(Code)), Msg);
	OnResponse.ExecuteIfBound(Code, {}, {}, {});
}

void UMatchmaker::UpdatePlayableTime()
{
	using namespace Matchmaker;

	Request(SSTR("GET"), GBaseURL + TEXT("/fleet/playTime"), [this](const int32 Code, const FJsonObject& Content)
	{
		if (Code != 200) return;

		FPeriod Time;
		if (!Content.TryGetNumberField(SSTR("startHour"), Time.Start.Hour)) return;
		if (!Content.TryGetNumberField(SSTR("startMinute"), Time.Start.Minute)) return;
		if (!Content.TryGetNumberField(SSTR("endHour"), Time.End.Hour)) return;
		if (!Content.TryGetNumberField(SSTR("endMinute"), Time.End.Minute)) return;
				
		Time.bIsSet = true;

		PlayableTime = MoveTemp(Time);
		SetPlayableTimeNotification();
	});
}

void UMatchmaker::SetPlayableTimeNotification()
{
	if (!PlayableTime.bIsSet)
	{
		UpdatePlayableTime();
		return;
	}

	if (!UUserSettings::Get(this)->IsNotificationEnabled()) return;
	if (FDateTime::Now() <= LastNotificationTime) return;
	
	const auto& P = PlayableTime;
	auto& S = P.Start;
	auto& E = P.End;
	
	const auto Hours = E.Hour - S.Hour;
	const auto Minutes = E.Minute - S.Minute;
	const FTimespan Duration{Hours, Minutes, 0};
	
	const auto Title = LOCTEXT("PlayableNotifyTitle", "이제 플레이할 수 있습니다!");
	const auto Body = FMT_MSG(LOCTEXT("PlayableNotifyBody", "지금부터 {0}동안 플레이할 수 있습니다! 놓지지 마세요!"), FText::AsTimespan(Duration));

	LastNotificationTime = USaucewich::ScheduleLocalNotificationAtTime(S.Hour, S.Minute, true, Title, Body, FText::GetEmpty(), {});
	SaveConfig();
}

#undef LOCTEXT_NAMESPACE
