// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichLibrary.h"

#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "SkeletalMeshMerge.h"

#include "Entity/ActorPool.h"
#include "Entity/DecalPoolActor.h"
#include "SaucewichGameInstance.h"

USkeletalMesh* USaucewichLibrary::MergeMeshes(const TArray<USkeletalMesh*>& Meshes)
{
	const auto BaseMesh = NewObject<USkeletalMesh>();
	FSkeletalMeshMerge Merger{BaseMesh, Meshes, {}, 0};
	if (Merger.DoMerge()) return BaseMesh;
	return nullptr;
}

ADecalPoolActor* SpawnPoolDecal(UMaterialInterface* const DecalMaterial, const FVector& DecalSize, UWorld* const World, const float LifeSpan)
{
	const auto Pool = USaucewichLibrary::GetActorPool(World);
	if (!Pool) return nullptr;

	const auto Decal = Pool->Spawn<ADecalPoolActor>();
	if (Decal)
	{
		const auto DecalComp = Decal->GetDecal();
		DecalComp->SetDecalMaterial(DecalMaterial);
		DecalComp->DecalSize = DecalSize;
		Decal->SetLifeSpan(LifeSpan);
	}
	
	return Decal;
}

ADecalPoolActor* USaucewichLibrary::SpawnPoolDecalAtLocation(const UObject* const WorldContextObject, UMaterialInterface* const DecalMaterial, const FVector DecalSize, const FVector Location, const FRotator Rotation, const float LifeSpan)
{
	if (!WorldContextObject) return nullptr;
	const auto World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	const auto Decal = SpawnPoolDecal(DecalMaterial, DecalSize, World, LifeSpan);
	if (Decal) Decal->SetActorLocationAndRotation(Location, Rotation);

	return Decal;
}

ADecalPoolActor* USaucewichLibrary::SpawnPoolDecalAttached(UMaterialInterface* const DecalMaterial, const FVector DecalSize, USceneComponent* const AttachToComponent, const FName AttachPointName, const FVector Location, const FRotator Rotation, const EAttachmentRule LocationType, const float LifeSpan)
{
	if (!AttachToComponent) return nullptr;
	const auto World = AttachToComponent->GetWorld();
	if (!World) return nullptr;

	const auto Decal = SpawnPoolDecal(DecalMaterial, DecalSize, World, LifeSpan);
	if (Decal)
	{
		Decal->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachPointName);
		if (LocationType == EAttachmentRule::KeepWorld)
		{
			Decal->SetActorLocationAndRotation(Location, Rotation);
		}
		else
		{
			Decal->SetActorRelativeLocation(Location);
			Decal->SetActorRelativeRotation(Rotation);
		}
	}

	return Decal;
}

AActorPool* USaucewichLibrary::GetActorPool(const UObject* const WorldContextObject)
{
	if (WorldContextObject)
		if (const auto World = WorldContextObject->GetWorld())
			if (const auto GI = World->GetGameInstance<USaucewichGameInstance>())
				return GI->GetActorPool();
	return nullptr;
}
