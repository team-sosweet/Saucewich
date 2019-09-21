// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "Http.h"
#include "HttpGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogExternalServer, Log, All)

USTRUCT(Atomic, BlueprintType)
struct FJson
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, class UJsonData*> Data;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnResponded, bool, IsSuccess, int32, Code, FJson, Json);

UCLASS()
class SAUCEWICH_API UHttpGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UHttpGameInstance();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GET"))
	void GetRequest(const FString& Url, FJson Json, const FOnResponded& OnResponded);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "POST"))
	void PostRequest(const FString& Url, FJson Json, const FOnResponded& OnResponded);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PUT"))
	void PutRequest(const FString& Url, FJson HeaderJson, FJson BodyJson, const FOnResponded& OnResponded);

	int32 GetMaxPort() const { return MaxPort; }
	int32 Port;
	
private:
	TSharedRef<IHttpRequest> CreateRequest(const FString& Url, const FOnResponded& OnResponded);
	
	bool GetStringFromJson(const FJson& Json, FString& Out) const;

	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	TMap<FString, FOnResponded> ResponseDelegates;

	class FHttpModule* Http;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FString BaseUrl;

	UPROPERTY(Config)
	int32 MaxPort = 8000;
};
