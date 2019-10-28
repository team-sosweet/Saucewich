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
	static class ADecalPoolActor* SpawnSauceDecal(
		const FHitResult& HitInfo,
		class UMaterialInterface* Material,
		const FLinearColor& Color,
		FVector SizeMin,
		FVector SizeMax,
		float LifeSpan = 10
	);

	UFUNCTION(BlueprintPure, meta=(WorldContext=WorldContextObject))
	static class AActorPool* GetActorPool(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContextObject))
	static void CleanupGame(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static ENameValidity IsValidPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMinLen();

	UFUNCTION(BlueprintPure)
	static int32 GetPlayerNameMaxLen();

	UFUNCTION(BlueprintPure)
	static class UUserSettings* GetUserSettings();

	UFUNCTION(BlueprintPure)
	static int32 GetServerVersion();

	static class FGameLiftServerSDKModule& GetGameLiftServerSDKModule();
};
