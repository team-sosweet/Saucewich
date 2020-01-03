// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "Saucewich.generated.h"

#define FMT_MSG(Fmt, ...) [&]{static const FTextFormat Compiled{Fmt};return FText::FormatOrdered(Compiled,__VA_ARGS__);}()

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogGameLift, Log, All)

class FJsonObject;
class FGameLiftServerSDKModule;
class IHttpRequest;

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

UENUM(BlueprintType)
enum class EHttpResponse : uint8
{
	OK, ConnectionFailed, Error
};

USTRUCT(BlueprintType)
struct SAUCEWICH_API FMatchmakingRequest
{
	GENERATED_BODY()

	FString TicketID;
	TSharedPtr<IHttpRequest> Handle;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FStartMatchmakingResponse, EHttpResponse, Response, const FString&, URL, const FString&, PlayerID);
DECLARE_DELEGATE_TwoParams(FOnHttpResponse, int32, const FJsonObject&)

namespace GameLift
{
	FGameLiftServerSDKModule& Get();
	void SafeTerminate();
	FString RandomString();
}

namespace Http
{
	EHttpResponse ToEnum(int32 Code);
}

UCLASS()
class SAUCEWICH_API USaucewich : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static bool CheckInputAction(FName ActionName, const struct FKeyEvent& KeyEvent);

	UFUNCTION(BlueprintCallable)
	static FMatchmakingRequest StartMatchmaking(const FStartMatchmakingResponse& Callback);
	
	UFUNCTION(BlueprintCallable)
	static void CancelMatchmaking(const FMatchmakingRequest& Handle);
	
	UFUNCTION(BlueprintPure)
	static ENameValidity IsValidPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMinLen();

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMaxLen();

	static TSharedRef<class IHttpRequest> Request(const FString& Verb, const FString& URL, const FOnHttpResponse& OnResponse);
	static TSharedRef<class IHttpRequest> Request(const FString& Verb, const FString& URL, const TSharedRef<FJsonObject>& Content, const FOnHttpResponse& OnResponse);
};
