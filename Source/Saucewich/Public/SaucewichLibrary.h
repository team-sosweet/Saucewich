// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "SaucewichLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Log, All)

UENUM(BlueprintType)
enum class ENameValidity : uint8
{
	Valid, Character, Length
};

UCLASS()
class SAUCEWICH_API USaucewichLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static class USkeletalMesh* MergeMeshes(const TArray<USkeletalMesh*>& Meshes);

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
};
