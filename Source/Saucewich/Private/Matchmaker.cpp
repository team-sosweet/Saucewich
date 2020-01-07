// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Matchmaker.h"
#include <chrono>
#include "Http.h"
#include "Json.h"
#include "Names.h"

DEFINE_LOG_CATEGORY_STATIC(LogMatchmaker, Log, All)

namespace Matchmaker
{
	static const FString GBaseURL = TEXT("http://api.saucewich.net");
	
	static TSharedRef<IHttpRequest> CreateRequest(const FString& Verb, const FString& URL)
	{
		const auto Request = FHttpModule::Get().CreateRequest();
		Request->SetVerb(Verb);
		Request->SetURL(URL);
		return Request;
	}

	template <class Fn>
	static TSharedRef<IHttpRequest> CreateRequest(const FString& Verb, const FString& URL, Fn&& OnResponse)
	{
		const auto Request = CreateRequest(Verb, URL);

		Request->OnProcessRequestComplete().BindLambda(
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

		return Request;
	}

	static const TSharedPtr<FJsonObject>& GetSessionInfo(const FJsonObject& Content)
	{
		auto&& Tickets = Content.GetArrayField(SSTR("TicketList"));
		if (Tickets.Num() > 0)
		{
			return Tickets[0]->AsObject()->GetObjectField(SSTR("GameSessionConnectionInfo"));
		}
		
		static const TSharedPtr<FJsonObject> Default = MakeShared<FJsonObject>();
		return Default;
	}

	static FString GetServerAddress(const FJsonObject& SessionInfo)
	{
		FString IP;
		if (!SessionInfo.TryGetStringField(SSTR("IpAddress"), IP)) return {};

		int32 Port;
		if (!SessionInfo.TryGetNumberField(SSTR("Port"), Port)) return {};

		IP += TEXT(':');
		IP.AppendInt(Port);
		return IP;
	}

	static const TSharedPtr<FJsonObject>& GetPlayer(const FJsonObject& SessionInfo)
	{
		static const TSharedPtr<FJsonObject> Default = MakeShared<FJsonObject>();
		
		const TArray<TSharedPtr<FJsonValue>>* PlayersPtr;
		if (!SessionInfo.TryGetArrayField(SSTR("MatchedPlayerSessions"), PlayersPtr)) return Default;
		auto&& Players = *PlayersPtr;

		if (Players.Num() == 0) return Default;

		return Players[0]->AsObject();
	}
}

UMatchmaker::UMatchmaker()
{
	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UMatchmaker::CancelMatchmaking, true);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UMatchmaker::CancelMatchmaking, true);
	FCoreDelegates::ApplicationWillTerminateDelegate.AddUObject(this, &UMatchmaker::CancelMatchmaking, true);
}

UMatchmaker::~UMatchmaker()
{
	CancelMatchmaking(true);
}

UMatchmaker* UMatchmaker::Get()
{
	static const auto Matchmaker = NewObject<UMatchmaker>(GetTransientPackage(), NAME_None, RF_MarkAsRootSet);
	return Matchmaker;
}

void UMatchmaker::StartMatchmaking()
{
	using namespace Matchmaker;
	using namespace std::chrono;
	
	Handle = CreateRequest(SSTR("GET"), GBaseURL + TEXT("/ping"),
		[this, StartTime=steady_clock::now()](const int32 Code, const FJsonObject&)
	{
		if (Code == 200)
		{
			const auto RoundTrip = steady_clock::now() - StartTime;
			const auto Latency = duration_cast<milliseconds>(RoundTrip / 2);
			OnPingComplete(static_cast<int32>(Latency.count()));
		}
		else
		{
			Error(EMMResponse::ConnFail);
		}
	});

	ProcessRequest();
}

void UMatchmaker::CancelMatchmaking(const bool bError)
{
	using namespace Matchmaker;

	if (Handle)
	{
		Handle->CancelRequest();
	}

	if (!TicketID.IsEmpty())
	{
		auto URL = GBaseURL;
		URL += TEXT("/match/cancel?ticketId=");
		URL += TicketID;
		
		CreateRequest(SSTR("DELETE"), URL)->ProcessRequest();
	}

	if (bError) Error(EMMResponse::Canceled);
	else Reset();
}

void UMatchmaker::BindCallback(const FOnStartMatchmakingResponse& Callback)
{
	OnResponse = Callback;
}

void UMatchmaker::OnPingComplete(const int32 LatencyInMs)
{
	using namespace Matchmaker;
	
	auto URL = GBaseURL;
	URL += TEXT("/match/start?ticketId=");
	URL += TicketID = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
	URL += TEXT("&LatencyInMs=");
	URL.AppendInt(LatencyInMs);
	
	Handle = CreateRequest(SSTR("GET"), URL, [this](const int32 Code, const FJsonObject& Content)
	{
		if (Code == 200)
		{
			OnMatchmakingComplete(Content);
		}
		else
		{
			const auto Desc = Content.GetStringField(SSTR("description"));
			
			if (Code == 0)
				Error(EMMResponse::ConnFail);
			
			else if (Desc == TEXT("TIMED_OUT"))
				Error(EMMResponse::Timeout);
			
			else if (Desc != TEXT("CANCELLED"))
				Error(EMMResponse::Error);
		}
	});
	
	ProcessRequest();
}

void UMatchmaker::OnMatchmakingComplete(const FJsonObject& Content)
{
	using namespace Matchmaker;

	const auto Address = Content.GetStringField(SSTR("Address"));
	const auto PlayerID = Content.GetStringField(SSTR("PlayerId"));
	const auto SessionID = Content.GetStringField(SSTR("PlayerSessionId"));

	if (!Address.IsEmpty() && !PlayerID.IsEmpty() && !SessionID.IsEmpty())
	{
		OnResponse.ExecuteIfBound(EMMResponse::OK, Address, PlayerID, SessionID);
		Reset();
	}
	else
	{
		Error(EMMResponse::Error);
	}
}

void UMatchmaker::ProcessRequest()
{
	if (!Handle->ProcessRequest())
	{
		Error(EMMResponse::ConnFail);
	}
}

void UMatchmaker::Error(const EMMResponse Code)
{
	OnResponse.ExecuteIfBound(Code, {}, {}, {});
	Reset();
}

void UMatchmaker::Reset()
{
	Handle.Reset();
	TicketID.Reset();
}
