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
	UFUNCTION(BlueprintCallable, meta=(UnsafeDuringActorConstruction=true))
	static class USkeletalMesh* MergeMeshes(const TArray<USkeletalMesh*>& Meshes);

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContextObject, UnsafeDuringActorConstruction=true))
	static class ADecalPoolActor* SpawnPoolDecalAtLocation(
		const UObject* WorldContextObject,
		class UMaterialInterface* DecalMaterial,
		FVector DecalSize,
		FVector Location,
		FRotator Rotation = FRotator(-90, 0, 0),
		float LifeSpan = 0
	);

	UFUNCTION(BlueprintCallable, meta=(UnsafeDuringActorConstruction=true))
	static class ADecalPoolActor* SpawnPoolDecalAttached(
		class UMaterialInterface* DecalMaterial,
		FVector DecalSize,
		class USceneComponent* AttachToComponent,
		FName AttachPointName = NAME_None,
		FVector Location = FVector::ZeroVector,
		FRotator Rotation = FRotator::ZeroRotator,
		EAttachmentRule LocationType = EAttachmentRule::KeepRelative,
		float LifeSpan = 0
	);

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContextObject, UnsafeDuringActorConstruction=true))
	static class AActorPool* GetActorPool(const UObject* WorldContextObject);
};
