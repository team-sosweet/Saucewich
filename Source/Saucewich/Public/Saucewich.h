// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "Saucewich.generated.h"

#define FMT_MSG(Fmt, ...) [&]{static const FTextFormat Compiled{Fmt};return FText::FormatOrdered(Compiled,__VA_ARGS__);}()

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogGameLift, Log, All)

class FJsonObject;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FSearchSessionResponse, bool, bFound, const FString&, URL, const FString&, PlayerID);
DECLARE_DELEGATE_TwoParams(FOnHttpResponse, int32, const FJsonObject&)

UENUM(BlueprintType)
enum class EGunTraceHit : uint8
{
	None, Pawn, Other
};

UENUM(BlueprintType)
enum class EMsgType : uint8
{
	Center, Left
};

UENUM(BlueprintType)
enum class ENameValidity : uint8
{
	Valid, Character, Length
};

UCLASS()
class SAUCEWICH_API USaucewich : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void SearchSession(const FSearchSessionResponse& Callback);
	
	UFUNCTION(BlueprintPure)
	static ENameValidity IsValidPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMinLen();

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMaxLen();

	static class FGameLiftServerSDKModule& GetGameLift();

	static void Request(const FString& Verb, const FString& URL, const FOnHttpResponse& OnResponse);
	static void Request(const FString& Verb, const FString& URL, const TSharedRef<FJsonObject>& Content, const FOnHttpResponse& OnResponse);
};
