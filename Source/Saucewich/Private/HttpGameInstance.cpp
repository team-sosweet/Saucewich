// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "HttpGameInstance.h"

#include "Json.h"
#include "JsonObjectConverter.h"

#include "JsonData.h"

DEFINE_LOG_CATEGORY(LogExternalServer)

UHttpGameInstance::UHttpGameInstance()
{
	Http = &FHttpModule::Get();
}

void UHttpGameInstance::GetRequest(const FString& Url, const FJson Json, const FOnResponded& OnResponded)
{
	FString Content;
	if (!GetStringFromJson(Json, Content))
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}

	const auto FinalUrl = BaseUrl + Url;
	if (!ResponseDelegates.Contains(FinalUrl))
	{
		const auto Request = CreateRequest(FinalUrl, OnResponded);
	
		TArray<FString> Params, Pair;
		Content.ParseIntoArray(Params, TEXT("&"));
		
		for (const auto& Param : Params)
		{
			Param.ParseIntoArray(Pair, TEXT("="));
			Request->SetHeader(Pair[0], Pair[1]);
		}
		
		Request->SetVerb("GET");
		Request->ProcessRequest();
	}
}

void UHttpGameInstance::PostRequest(const FString& Url, const FJson Json, const FOnResponded& OnResponded)
{
	FString Content;
	if (!GetStringFromJson(Json, Content))
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}

	const auto FinalUrl = BaseUrl + Url;
	if (!ResponseDelegates.Contains(FinalUrl))
	{
		const auto Request = CreateRequest(FinalUrl, OnResponded);
		Request->SetVerb("POST");
		Request->SetContentAsString(Content);
		Request->ProcessRequest();
	}
}

void UHttpGameInstance::PutRequest(const FString& Url, const FJson HeaderJson, const FJson BodyJson, const FOnResponded& OnResponded)
{
	FString HeaderContent;
	if (!GetStringFromJson(HeaderJson, HeaderContent))
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}

	FString BodyContent;
	if (!GetStringFromJson(HeaderJson, BodyContent))
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}

	const auto FinalUrl = BaseUrl + Url;
	if (ResponseDelegates.Contains(FinalUrl))
	{
		OnResponded.ExecuteIfBound(false, 429, {});
		return;
	}

	const auto Request = CreateRequest(FinalUrl, OnResponded);

	TArray<FString> Params, Pair;
	HeaderContent.ParseIntoArray(Params, TEXT("&"));

	for (const auto& Param : Params)
	{
		Param.ParseIntoArray(Pair, TEXT("="));
		Request->SetHeader(Pair[0], Pair[1]);
	}

	if (!BodyContent.IsEmpty())
	{
		Request->SetContentAsString(BodyContent);
	}

	Request->SetVerb("PUT");
	Request->ProcessRequest();
}

TSharedRef<IHttpRequest> UHttpGameInstance::CreateRequest(const FString& Url, const FOnResponded& OnResponded)
{
	const auto Request = Http->CreateRequest();
	ResponseDelegates.Add(Url, OnResponded);
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpGameInstance::OnResponse);
	
	Request->SetURL(Url);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	return Request;
}

void UHttpGameInstance::OnResponse(const FHttpRequestPtr Request, const FHttpResponsePtr Response, const bool bWasSuccessful)
{
	const auto OnResponded = ResponseDelegates.FindAndRemoveChecked(Request->GetURL());
	
	if (!bWasSuccessful || !Response.IsValid())
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}
	
	const auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonObject> JsonObject;
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		OnResponded.ExecuteIfBound(false, 403, {});
		return;
	}

	TMap<FString, UJsonData*> JsonData;
	
	for (const auto& Datum : JsonObject->Values)
	{
		auto Value = NewObject<UJsonData>();
		Value->Create(Datum.Value);
		JsonData.Add(Datum.Key, Value);
	}

	OnResponded.ExecuteIfBound(true, Response->GetResponseCode(), {JsonData});
}

bool UHttpGameInstance::GetStringFromJson(const FJson& Json, FString& Out) const
{
	auto IsContinuous = false;
	
	for (const auto& Elem : Json.Data)
	{
		FString ValueStr;

		const auto Value = **Elem.Value;
		switch (Value->Type)
		{
		case EJson::Boolean:
			ValueStr = Value->AsBool() ? TEXT("true") : TEXT("false");
			break;

		case EJson::Number:
			ValueStr = FString::SanitizeFloat(Value->AsNumber());
			break;

		case EJson::String:
			ValueStr = Value->AsString();
			break;

		default:
			return false;
		}

		if (IsContinuous)
		{
			Out += TEXT("&");
		}
		else
		{
			IsContinuous = true;
		}
		
		Out += FString::Printf(TEXT("%s=%s"), *Elem.Key, *ValueStr);
	}

	return true;
}
