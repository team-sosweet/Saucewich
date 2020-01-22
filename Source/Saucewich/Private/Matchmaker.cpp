// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Matchmaker.h"
#include <chrono>
#include "Http.h"
#include "Json.h"
#include "Names.h"

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
	
	auto URL = GBaseURL;
	URL += TEXT("/match/start?AliasId=alias-53ca2617-e3b1-4c0a-a0e6-a0721b1f8176");
	
	Handle = CreateRequest(SSTR("GET"), URL, [this](const int32 Code, const FJsonObject& Content)
	{
		if (Code == 200)
		{
			OnMatchmakingComplete(Content);
		}
		else
		{
			if (Code == 0) Error(EMMResponse::ConnFail);
			else Error(EMMResponse::Error);
		}
	});

	ProcessRequest();
}

void UMatchmaker::BindCallback(const FOnStartMatchmakingResponse& Callback)
{
	OnResponse = Callback;
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
}
