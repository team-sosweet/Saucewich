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

void UHttpGameInstance::GetRequest(const FString& URL, const FOnResponded& OnResponded)
{
	auto Request = CreateRequest(URL, OnResponded);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UHttpGameInstance::PostRequest(const FString& URL, const FJson& Json, const FOnResponded& OnResponded)
{
	auto Request = CreateRequest(URL, OnResponded);
	Request->SetVerb("POST");

	TArray<TSharedPtr<FJsonValue>> JsonValues;

	for (const auto& Pair : DeserializeJson(Json.Data))
	{
		JsonValues.Add(Pair.Value);
	}
	
	FString JsonString;
	const auto Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonValues, Writer);
	Writer->Close();

	Request->SetContentAsString(JsonString);
	Request->ProcessRequest();
}

TSharedRef<IHttpRequest> UHttpGameInstance::CreateRequest(const FString& URL, const FOnResponded& OnResponded)
{
	const auto Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UHttpGameInstance::OnResponse);
	ResponseDelegates.Add(URL, OnResponded);

	Request->SetURL(BaseURL + URL);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	return Request;
}

TMap<FString, UJsonData*> UHttpGameInstance::SerializeJson(const TMap<FString, TSharedPtr<FJsonValue>>& Data) const
{
	TMap<FString, UJsonData*> Ret;

	for (const auto& Datum : Data)
	{
		auto Value = NewObject<UJsonData>();
		Value->Create(Datum.Value);
		Ret.Add(Datum.Key, Value);
	}

	return Ret;
}

TMap<FString, TSharedPtr<FJsonValue>> UHttpGameInstance::DeserializeJson(const TMap<FString, UJsonData*>& Json)
{
	TMap<FString, TSharedPtr<FJsonValue>> Ret;

	for (const auto& Pair : Json)
	{
		Ret.Add(Pair.Key, **Pair.Value);
	}

	return Ret;
}

void UHttpGameInstance::OnResponse(const FHttpRequestPtr Request, const FHttpResponsePtr Response, const bool bWasSuccessful)
{
	const auto Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(Reader, JsonObject);

	const auto Json = FJson{ SerializeJson(JsonObject->Values) };
	ResponseDelegates.FindAndRemoveChecked(Request->GetURL()).ExecuteIfBound(Json);
}