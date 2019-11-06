// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"

#include "UserSettings.h"

// 컴파일러의 버그인지 모르겠지만 가끔 아래 static_assert가 실패하는 말도 안되는 일이 발생한다
#include "DecalPoolActor.h"
#include "ActorPool.h"
static_assert(TPointerIsConvertibleFromTo<APoolActor, AActor>::Value, "APoolActor* is not convertible to AActor*");

#if WITH_GAMELIFT

	#include "GameLiftServerSDK.h"
	DEFINE_LOG_CATEGORY(LogGameLift)

	FGameLiftServerSDKModule& USaucewich::GetGameLift()
	{
		static auto& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
		return Module;
	}

#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Saucewich, "Saucewich")

DEFINE_LOG_CATEGORY(LogSaucewich)

ADecalPoolActor* USaucewich::SpawnSauceDecal(const FHitResult& HitInfo, UMaterialInterface* const Material, const FLinearColor& Color,
	const FVector SizeMin, const FVector SizeMax, const float LifeSpan)
{
	const auto Comp = HitInfo.GetComponent();
	if (!Comp || Comp->Mobility != EComponentMobility::Static) return nullptr;
	
	const auto World = Comp->GetWorld();
	const auto Pool = AActorPool::Get(World);

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

void USaucewich::CleanupGame(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return;
	const auto World = WorldContextObject->GetWorld();
	if (!World) return;

	for (const auto Actor : TActorRange<APoolActor>{World})
		Actor->Release();
}

ENameValidity USaucewich::IsValidPlayerName(const FString& PlayerName)
{
	if (PlayerName.Len() < GetPlayerNameMinLen() || PlayerName.Len() > GetPlayerNameMaxLen())
		return ENameValidity::Length;

	for (const auto c : PlayerName)
		if (!FChar::IsIdentifier(c))
			return ENameValidity::Character;

	return ENameValidity::Valid;
}

int32 USaucewich::GetPlayerNameMinLen()
{
	return 2;
}

int32 USaucewich::GetPlayerNameMaxLen()
{
	return 16;
}
