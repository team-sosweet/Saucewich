// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshMergeFunctionLibrary.generated.h"

UCLASS()
class SAUCEWICH_API UMeshMergeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(UnsafeDuringActorConstruction=true))
	static class USkeletalMesh* MergeMeshes(const TArray<USkeletalMesh*>& Meshes);
};
