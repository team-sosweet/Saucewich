// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "Matchmaker.generated.h"

UENUM(BlueprintType)
enum class EMMResponse : uint8
{
	OK,
	ConnFail,
	Error,
	Outdated,
	NotPlayableTime
};

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnStartMatchmakingResponse,
	EMMResponse, Response, const FString&, URL, const FString&, PlayerID, const FString&, SessionID);

USTRUCT(BlueprintType)
struct FTime
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 Hour;
	
	UPROPERTY(BlueprintReadWrite)
	int32 Minute;
};

USTRUCT(BlueprintType)
struct FPeriod
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	uint8 bIsSet : 1;

	UPROPERTY(BlueprintReadWrite)
	FTime Start;

	UPROPERTY(BlueprintReadWrite)
	FTime End;
};

UCLASS(Config=UserSettings)
class SAUCEWICH_API UMatchmaker : public UObject
{
	GENERATED_BODY()

public:
	UMatchmaker();
	~UMatchmaker();

	UFUNCTION(BlueprintPure, meta=(DisplayName="Get Matchmaker", WorldContext=W))
	static UMatchmaker* Get(const UObject* W);

	UFUNCTION(BlueprintCallable)
	void StartMatchmaking();

	UFUNCTION(BlueprintCallable)
	void BindCallback(const FOnStartMatchmakingResponse& Callback);

private:
	void OnMatchmakingComplete(const class FJsonObject& Content) const;
	void Error(EMMResponse Code) const;

	void UpdatePlayableTime();
	void SetPlayableTimeNotification();

	UPROPERTY(Config)
	FDateTime LastNotificationTime;

	UPROPERTY(BlueprintReadOnly, Transient, meta=(AllowPrivateAccess=true))
	FPeriod PlayableTime;

	FOnStartMatchmakingResponse OnResponse;
};
