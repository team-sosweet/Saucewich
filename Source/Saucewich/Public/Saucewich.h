// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "Saucewich.generated.h"

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

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Log, All)

#if WITH_GAMELIFT
	DECLARE_LOG_CATEGORY_EXTERN(LogGameLift, Log, All)
#endif

#define FMT_MSG(Fmt, ...) [&]{static const FTextFormat Compiled{Fmt};return FText::FormatOrdered(Compiled,__VA_ARGS__);}()

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
	UFUNCTION(BlueprintPure)
	static ENameValidity IsValidPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMinLen();

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMaxLen();

	static class FGameLiftServerSDKModule& GetGameLift();
};
