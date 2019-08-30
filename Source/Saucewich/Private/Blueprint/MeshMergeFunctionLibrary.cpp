// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Blueprint/MeshMergeFunctionLibrary.h"

#include "SkeletalMeshMerge.h"

#include "Saucewich.h"

USkeletalMesh* UMeshMergeFunctionLibrary::MergeMeshes(const TArray<USkeletalMesh*>& Meshes)
{
	const auto BaseMesh = NewObject<USkeletalMesh>();
	FSkeletalMeshMerge Merger{BaseMesh, Meshes, {}, 0};
	if (GUARANTEE(Merger.DoMerge())) return BaseMesh;
	return nullptr;
}
