// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Matchmaker.h"
#include <chrono>
#include "Http.h"
#include "Json.h"
#include "Names.h"
#include "Saucewich.h"
#include "UserSettings.h"
#include "SaucewichInstance.h"

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
	UpdatePlayableTime();
	
	const auto Delegate = TBaseDelegate<void>::CreateUObject(this, &UMatchmaker::SetPlayableTimeNotification);
	FCoreDelegates::ApplicationWillDeactivateDelegate.Add(Delegate);
	FCoreDelegates::ApplicationHasReactivatedDelegate.Add(Delegate);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.Add(Delegate);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.Add(Delegate);
	FCoreDelegates::ApplicationWillTerminateDelegate.Add(Delegate);

	UUserSettings::Get(this)->OnNotificationDisabled.AddWeakLambda(this, [this]
	{
		LastNotificationTime = {};
		SaveConfig();
	});
}

UMatchmaker::~UMatchmaker()
{
	SetPlayableTimeNotification();
}

UMatchmaker* UMatchmaker::Get(const UObject* const W)
{
	return USaucewichInstance::Get(W)->GetMatchmaker();
}

void UMatchmaker::StartMatchmaking()
{
	using namespace Matchmaker;
	using namespace std::chrono;

	const auto URL = GBaseURL + TEXT("/match/start?AliasId=alias-53ca2617-e3b1-4c0a-a0e6-a0721b1f8176");
	
	const auto bSuccess = Request(SSTR("GET"), URL, [this](const int32 Code, const FJsonObject& Content)
	{
		switch (Code)
		{
		case 200:
			OnMatchmakingComplete(Content);
			break;
		case 500:
			if (Content.GetStringField(SSTR("description")).Contains(TEXT("OUTDATED"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
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
		Error(EMMResponse::Error);
	}
}

void UMatchmaker::Error(const EMMResponse Code) const
{
	OnResponse.ExecuteIfBound(Code, {}, {}, {});
}

void UMatchmaker::UpdatePlayableTime()
{
	using namespace Matchmaker;

	Request(SSTR("GET"), GBaseURL + TEXT("/playTime"), [this](const int32 Code, const FJsonObject& Content)
	{
		if (Code != 200) return;

		FPeriod Time;
		if (!Content.TryGetNumberField(SSTR("startHour"), Time.Start.Hour)) return;
		if (!Content.TryGetNumberField(SSTR("startMinute"), Time.Start.Minute)) return;
		if (!Content.TryGetNumberField(SSTR("endHour"), Time.End.Hour)) return;
		if (!Content.TryGetNumberField(SSTR("endMinute"), Time.End.Minute)) return;
		
		auto ToUtc = [](int32& Hour)
		{
			// 서버가 반환하는 플레이 가능 시간은 서버 로컬 한국시간 (UTC+9) 기준이다.
			Hour = (24 - 9 + Hour) % 24;
		};

		ToUtc(Time.Start.Hour);
		ToUtc(Time.End.Hour);
		
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
	if (FDateTime::UtcNow() <= LastNotificationTime) return;
	
	const auto& P = PlayableTime;
	auto& S = P.Start;
	auto& E = P.End;
	
	const auto Hours = E.Hour - S.Hour;
	const auto Minutes = E.Minute - S.Minute;
	const FTimespan Duration{Hours, Minutes, 0};
	
	const auto Title = LOCTEXT("PlayableNotifyTitle", "이제 플레이할 수 있습니다!");
	const auto Body = FText::FormatOrdered(
		LOCTEXT("PlayableNotifyBody", "지금부터 {0}동안 플레이할 수 있습니다! 놓지지 마세요!"), FText::AsTimespan(Duration));

	USaucewich::ScheduleLocalNotificationAtTime(S.Hour, S.Minute, false, Title, Body, FText::GetEmpty(), {});
	
	LastNotificationTime = FDateTime::UtcNow();
	SaveConfig();
}

#undef LOCTEXT_NAMESPACE
