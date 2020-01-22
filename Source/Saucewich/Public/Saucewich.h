// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "Saucewich.generated.h"

#define FMT_MSG(Fmt, ...) [&]{static const FTextFormat Compiled{Fmt};return FText::FormatOrdered(Compiled,__VA_ARGS__);}()

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(LogGameLift, Log, All)

class FGameLiftServerSDKModule;

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

namespace GameLift
{
	FGameLiftServerSDKModule& Get();
	void SafeTerminate();
}

UCLASS()
class SAUCEWICH_API USaucewich : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static bool CheckInputAction(FName ActionName, const struct FKeyEvent& KeyEvent);

	UFUNCTION(BlueprintPure)
	static ENameValidity IsValidPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMinLen();

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMaxLen();
};
