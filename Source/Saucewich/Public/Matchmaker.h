// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Matchmaker.generated.h"

UENUM(BlueprintType)
enum class EMMResponse : uint8
{
	OK, ConnFail, Error, Timeout
};

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnStartMatchmakingResponse, EMMResponse, Response, const FString&, URL, const FString&, PlayerID, const FString&, SessionID);

UCLASS()
class SAUCEWICH_API UMatchmaker : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Matchmaker"))
	static UMatchmaker* Get();
	
	UFUNCTION(BlueprintCallable)
	void StartMatchmaking();
	
	UFUNCTION(BlueprintCallable)
	void CancelMatchmaking();

	UFUNCTION(BlueprintCallable)
	void BindCallback(const FOnStartMatchmakingResponse& Callback);

private:
	void OnPingComplete(int32 LatencyInMs);
	void OnMatchmakingComplete(const class FJsonObject& Content);
	void ProcessRequest();
	void Error(EMMResponse Code);
	void Reset();
	
	FString TicketID;
	TSharedPtr<class IHttpRequest> Handle;
	FOnStartMatchmakingResponse OnResponse;
};
