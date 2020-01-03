// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"
#include "Http.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "HttpManager.h"
#include "GameFramework/InputSettings.h"
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

FString GameLift::RandomString()
{
	static const TCHAR Chars[] = TEXT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.");
	constexpr auto Len = 128;
	FString Str;
	Str.Reserve(Len);
	for (auto i=0; i<Len; ++i)
		Str += Chars[FMath::RandHelper(std::extent_v<decltype(Chars)>-1)];
	return Str;
}

EHttpResponse Http::ToEnum(const int32 Code)
{
	switch (Code)
	{
	case 200: return EHttpResponse::OK;
	case 0: return EHttpResponse::ConnectionFailed;
	default: return EHttpResponse::Error;
	}
}

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

static const FString GServerURL = TEXT("http://localhost:3000");

FMatchmakingRequest USaucewich::StartMatchmaking(const FStartMatchmakingResponse& Callback)
{
	auto TicketID = GameLift::RandomString();
	const auto URL = GServerURL + TEXT("/match/start?ticketId=") + TicketID;

	const auto Handle = Request(SSTR("GET"), URL, FOnHttpResponse::CreateLambda(
		[=](const int32 Code, const FJsonObject& Content)
		{
			if (Code == 200)
			{
				auto&& SessionInfo = Content.GetArrayField(SSTR("GameSessionConnectionInfo"))[0]->AsObject();
				auto ServerIP = SessionInfo->GetStringField(SSTR("IpAddress"));
				ServerIP += SessionInfo->GetStringField(SSTR("Port"));
				const auto PlayerID = Content.GetArrayField(SSTR("Players"))[0]->AsObject()->GetStringField(SSTR("PlayerId"));
				if (!PlayerID.IsEmpty() && !ServerIP.IsEmpty())
				{
					Callback.Execute(EHttpResponse::OK, ServerIP, PlayerID);
				}
				else
				{
					Callback.Execute(EHttpResponse::Error, {}, {});
				}
			}
			else
			{
				Callback.Execute(Http::ToEnum(Code), {}, {});
			}
		}
	));

	return {MoveTemp(TicketID), Handle};
}

void USaucewich::CancelMatchmaking(const FMatchmakingRequest& Handle)
{
	if (Handle.Handle)
	{
		Handle.Handle->CancelRequest();
	}

	if (!Handle.TicketID.IsEmpty())
	{
		const auto URL = GServerURL + TEXT("/match/cancel?ticketId=") + Handle.TicketID;
		Request(SSTR("DELETE"), URL, {});
	}
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

static TSharedRef<IHttpRequest> CreateRequest(const FString& Verb, const FString& URL, const FOnHttpResponse& OnResponse)
{
	const auto Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(Verb);
	Request->SetURL(URL);

	Request->OnProcessRequestComplete().BindLambda(
		[=](const FHttpRequestPtr&, const FHttpResponsePtr& Response, const bool bConnectedSuccessfully)
		{
			if (bConnectedSuccessfully)
			{
				const auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				TSharedPtr<FJsonObject> JsonObject;
				FJsonSerializer::Deserialize(Reader, JsonObject);
				static const FJsonObject DefaultJson;
				(void)OnResponse.ExecuteIfBound(Response->GetResponseCode(), JsonObject ? *JsonObject : DefaultJson);
			}
			else
			{
				OnResponse.ExecuteIfBound(0, {});
			}
		}
	);

	return Request;
}

TSharedRef<IHttpRequest> USaucewich::Request(const FString& Verb, const FString& URL, const FOnHttpResponse& OnResponse)
{
	const auto Request = CreateRequest(Verb, URL, OnResponse);
	if (!Request->ProcessRequest()) OnResponse.ExecuteIfBound(0, {});
	return Request;
}

TSharedRef<IHttpRequest> USaucewich::Request(const FString& Verb, const FString& URL, const TSharedRef<FJsonObject>& Content, const FOnHttpResponse& OnResponse)
{
	const auto Request = CreateRequest(Verb, URL, OnResponse);
	
	FString Serialized;
	const auto Writer = TJsonWriterFactory<>::Create(&Serialized);
	if (FJsonSerializer::Serialize(Content, Writer))
	{
		Request->SetContentAsString(Serialized);
	}

	if (!Request->ProcessRequest()) OnResponse.ExecuteIfBound(0, {});
	return Request;
}
