// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichLibrary.h"


#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "SkeletalMeshMerge.h"

#include "Entity/ActorPool.h"
#include "Entity/DecalPoolActor.h"
#include "SaucewichGameInstance.h"
#include "UserSettings.h"

DEFINE_LOG_CATEGORY(LogSaucewich)

#if WITH_GAMELIFT

	#include "GameLiftServerSDK.h"
	DEFINE_LOG_CATEGORY(LogGameLift)

	FGameLiftServerSDKModule& USaucewichLibrary::GetGameLiftServerSDKModule()
	{
		static auto& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
		return Module;
	}

#endif

USkeletalMesh* USaucewichLibrary::MergeMeshes(const TArray<USkeletalMesh*>& Meshes)
{
	const auto BaseMesh = NewObject<USkeletalMesh>();
	FSkeletalMeshMerge Merger{BaseMesh, Meshes, {}, 0};
	if (Merger.DoMerge()) return BaseMesh;
	return nullptr;
}

ADecalPoolActor* USaucewichLibrary::SpawnSauceDecal(const FHitResult& HitInfo, UMaterialInterface* const Material, const FLinearColor& Color,
	const FVector SizeMin, const FVector SizeMax, const float LifeSpan)
{
	const auto Comp = HitInfo.GetComponent();
	if (!Comp || Comp->Mobility != EComponentMobility::Static) return nullptr;
	
	const auto World = Comp->GetWorld();
	if (!World) return nullptr;

	const auto Pool = GetActorPool(World);
	if (!Pool) return nullptr;

	FHitResult ComplexHit;
	const auto bHitComplex = World->LineTraceSingleByChannel(ComplexHit, HitInfo.ImpactPoint + HitInfo.ImpactNormal * .1, HitInfo.ImpactPoint - HitInfo.ImpactNormal * 10, ECC_Visibility, {NAME_None, true});
	auto& ActualHit = bHitComplex ? ComplexHit : HitInfo;

	auto Rot = ActualHit.ImpactNormal.Rotation();
	Rot.Roll = FMath::FRandRange(0, 360);
	
	const auto Decal = Pool->Spawn<ADecalPoolActor>({Rot, ActualHit.ImpactPoint});
	if (Decal)
	{
		Decal->SetDecalMaterial(Material);
		Decal->SetColor(Color);
		Decal->SetDecalSize({
			FMath::RandRange(SizeMin.X, SizeMax.X),
			FMath::RandRange(SizeMin.Y, SizeMax.Y),
			FMath::RandRange(SizeMin.Z, SizeMax.Z)
		});
		Decal->SetLifeSpan(LifeSpan);
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

void USaucewichLibrary::CleanupGame(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return;
	const auto World = WorldContextObject->GetWorld();
	if (!World) return;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APoolActor::StaticClass(), Actors);
	for (const auto Actor : Actors)
		CastChecked<APoolActor>(Actor)->Release();
}

ENameValidity USaucewichLibrary::IsValidPlayerName(const FString& PlayerName)
{
	auto Len = 0;
	for (const auto C : PlayerName)
	{
		if (isalnum(C) || C == '_') Len += 1;
		else if (TEXT('가') <= C && C <= TEXT('힣')) Len += 2;
		else return ENameValidity::Character;
		if (Len > GetPlayerNameMaxLen()) return ENameValidity::Length;
	}
	return Len >= GetPlayerNameMinLen() ? ENameValidity::Valid : ENameValidity::Length;
}

int32 USaucewichLibrary::GetPlayerNameMinLen()
{
	return 1;
}

int32 USaucewichLibrary::GetPlayerNameMaxLen()
{
	return 16;
}

UUserSettings* USaucewichLibrary::GetUserSettings()
{
	static const auto UserSettings = NewObject<UUserSettings>(GetTransientPackage(), NAME_None, RF_MarkAsRootSet);
	return UserSettings;
}

int32 USaucewichLibrary::GetServerVersion()
{
	return 1;
}
