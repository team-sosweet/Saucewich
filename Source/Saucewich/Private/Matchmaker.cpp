// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Matchmaker.h"
#include "Http.h"
#include "Json.h"
#include "Names.h"

namespace Matchmaker
{
	static constexpr auto GBaseURL = TEXT("http://localhost:3000");
	
	static FString RandomString()
	{
		static const TCHAR Chars[] = TEXT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.");
		constexpr auto Len = 128;
		FString Str;
		Str.Reserve(Len);
		for (auto i=0; i<Len; ++i) Str += Chars[FMath::RandHelper(std::extent_v<decltype(Chars)>-1)];
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
}

UMatchmaker* UMatchmaker::Get()
{
	static const auto Matchmaker = NewObject<UMatchmaker>(GetTransientPackage(), NAME_None, RF_MarkAsRootSet);
	return Matchmaker;
}

void UMatchmaker::StartMatchmaking()
{
	using namespace Matchmaker;
	
	FString URL = GBaseURL;
	URL.Append(TEXT("/match/start?ticketId=")).Append(TicketID = RandomString());

	Handle = CreateRequest(SSTR("GET"), URL, [this](const int32 Code, const TSharedPtr<FJsonObject>& Content)
		{
			if (Code == 200 && Content)
			{
				auto&& SessionInfo = Content->GetArrayField(SSTR("GameSessionConnectionInfo"))[0]->AsObject();
				auto ServerIP = SessionInfo->GetStringField(SSTR("IpAddress"));
				ServerIP += TEXT(':');
				ServerIP += SessionInfo->GetStringField(SSTR("Port"));
				const auto PlayerID = Content->GetArrayField(SSTR("Players"))[0]->AsObject()->GetStringField(SSTR("PlayerId"));
				OnResponse.ExecuteIfBound(EMatchmakingResponse::OK, ServerIP, PlayerID);
			}
			else
			{
				OnResponse.ExecuteIfBound(Code == 0 ? EMatchmakingResponse::ConnectionFailed : EMatchmakingResponse::Error, {}, {});
			}
		
			TicketID.Reset();
			Handle.Reset();
		}
	);

	if (!Handle->ProcessRequest())
	{
		OnResponse.ExecuteIfBound(EMatchmakingResponse::ConnectionFailed, {}, {});
	}
}

void UMatchmaker::CancelMatchmaking()
{
	using namespace Matchmaker;

	if (Handle)
	{
		Handle->CancelRequest();
		Handle.Reset();
	}

	if (!TicketID.IsEmpty())
	{
		FString URL = GBaseURL;
		URL += TEXT("/match/cancel?ticketId=");
		URL += TicketID;
		
		CreateRequest(SSTR("DELETE"), URL)->ProcessRequest();

		TicketID.Reset();
	}
}

void UMatchmaker::BindCallback(const FOnStartMatchmakingResponse& Callback)
{
	OnResponse = Callback;
}
