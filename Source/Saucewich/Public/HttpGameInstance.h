// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "Json.h"
#include "JsonData.h"
#include "HttpGameInstance.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FJson
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UJsonData*> Data;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnResponded, FJson, Json);

UCLASS()
class SAUCEWICH_API UHttpGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UHttpGameInstance();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GET"))
	void GetRequest(const FString& URL, const FOnResponded& OnResponded);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "POST"))
	void PostRequest(const FString& URL, const FJson& Json, const FOnResponded& OnResponded);

private:
	TSharedRef<class IHttpRequest> CreateRequest(const FString& URL, const FOnResponded& OnResponded);

	TMap<FString, UJsonData*> SerializeJson(const TMap<FString, TSharedPtr<FJsonValue>>& Data) const;
	static TMap<FString, TSharedPtr<FJsonValue>> DeserializeJson(const TMap<FString, UJsonData*>& Json);
	
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	TMap<FString, FOnResponded> ResponseDelegates;

	class FHttpModule* Http;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FString BaseURL;
};
