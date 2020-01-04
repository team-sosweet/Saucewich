// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Matchmaker.generated.h"

UENUM(BlueprintType)
enum class EMatchmakingResponse : uint8
{
	OK, ConnectionFailed, Error
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnStartMatchmakingResponse, EMatchmakingResponse, Response, const FString&, URL, const FString&, PlayerID);

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
	FString TicketID;
	TSharedPtr<class IHttpRequest> Handle;
	FOnStartMatchmakingResponse OnResponse;
};
