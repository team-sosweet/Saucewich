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
	
	static FString RandomString()
	{
		static const TCHAR Chars[] = TEXT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.");
		constexpr auto NumChar = std::extent<decltype(Chars)>::value - 1;
		constexpr auto Len = 128;
		FString Str;
		Str.Reserve(Len);
		for (auto i=0; i<Len; ++i) Str += Chars[FMath::RandHelper(NumChar)];
		return Str;
	}

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
				OnResponse(Response->GetResponseCode(), JsonObject);
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

	static FString GetPlayerID(const FJsonObject& SessionInfo)
	{
		const TArray<TSharedPtr<FJsonValue>>* PlayersPtr;
		if (!SessionInfo.TryGetArrayField(SSTR("MatchedPlayerSessions"), PlayersPtr)) return {};
		auto&& Players = *PlayersPtr;

		if (Players.Num() == 0) return {};

		const TSharedPtr<FJsonObject>* PlayerPtr;
		if (!Players[0]->TryGetObject(PlayerPtr)) return {};
		auto&& Player = **PlayerPtr;

		FString ID;
		Player.TryGetStringField(SSTR("PlayerSessionId"), ID);
		return ID;
	}
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
		[this, StartTime=steady_clock::now()](const int32 Code, const TSharedPtr<FJsonObject>&)
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

void UMatchmaker::CancelMatchmaking()
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

	Reset();
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
	URL += TicketID = RandomString();
	URL += TEXT("&LatencyInMs=");
	URL.AppendInt(LatencyInMs);
	
	Handle = CreateRequest(SSTR("GET"), URL, [this](const int32 Code, const TSharedPtr<FJsonObject>& Content)
	{
		if (Code == 200 && Content)
		{
			OnMatchmakingComplete(*Content);
		}
		else
		{
			UE_LOG(LogMatchmaker, Error, TEXT("/match/start responded %d"), Code);
			Error(Code == 0 ? EMMResponse::ConnFail : EMMResponse::Error);
		}
	});
	
	ProcessRequest();
}

void UMatchmaker::OnMatchmakingComplete(const FJsonObject& Content)
{
	using namespace Matchmaker;

	auto&& SessionInfo = *GetSessionInfo(Content);
	const auto Address = GetServerAddress(SessionInfo);
	const auto PlayerID = GetPlayerID(SessionInfo);

	if (!Address.IsEmpty() && !PlayerID.IsEmpty())
	{
		OnResponse.ExecuteIfBound(EMMResponse::OK, Address, PlayerID);
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
	OnResponse.ExecuteIfBound(Code, {}, {});
	Reset();
}

void UMatchmaker::Reset()
{
	Handle.Reset();
	TicketID.Reset();
}
