// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/EngineTypes.h"
#include "SaucewichLibrary.generated.h"

UCLASS()
class SAUCEWICH_API USaucewichLibrary final : public UBlueprintFunctionLibrary
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

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContextObject))
	static class AActorPool* GetActorPool(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContextObject))
	static void CleanupGame(const UObject* WorldContextObject);
};
