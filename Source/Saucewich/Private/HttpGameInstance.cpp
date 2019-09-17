// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "HttpGameInstance.h"

#include "Http.h"
#include "Json.h"
#include "JsonObjectConverter.h"

#include "JsonData.h"

UHttpGameInstance::UHttpGameInstance()
{
	Http = &FHttpModule::Get();
}

void UHttpGameInstance::GetRequest(const FString& Url, const FOnResponded& OnResponded)
{
	auto Request = CreateRequest(Url, OnResponded);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UHttpGameInstance::PostRequest(const FString& Url, const FJson& Json, const FOnResponded& OnResponded)
{
	if (ResponseDelegates.Contains(Url))
	{
		return;
	}
	
	FString Content;
	if (!GetStringFromJson(Json, Content))
	{
		OnResponded.ExecuteIfBound(false, FJson());
		return;
	}
	
	auto Request = CreateRequest(Url, OnResponded);
	Request->SetVerb("POST");

	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

TSharedRef<IHttpRequest> UHttpGameInstance::CreateRequest(const FString& Url, const FOnResponded& OnResponded)
{
	const auto Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpGameInstance::OnResponse);

	const auto FinalUrl = BaseUrl + Url;
	
	ResponseDelegates.Add(FinalUrl, OnResponded);
	Request->SetURL(FinalUrl);
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
		OnResponded.ExecuteIfBound(false, FJson());
		return;
	}
	
	const auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(Reader, JsonObject);

	TMap<FString, UJsonData*> JsonData;
	
	for (const auto& Datum : JsonObject->Values)
	{
		auto Value = NewObject<UJsonData>();
		Value->Create(Datum.Value);
		JsonData.Add(Datum.Key, Value);
	}
	
	const auto Json = FJson{ JsonData };
	OnResponded.ExecuteIfBound(true, Json);
}

bool UHttpGameInstance::GetStringFromJson(const FJson& Json, FString& Out)
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