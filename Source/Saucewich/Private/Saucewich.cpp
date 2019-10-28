// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Saucewich.h"

#include "EngineUtils.h"
#include "Modules/ModuleManager.h"

#include "SaucewichGameInstance.h"
#include "UserSettings.h"

// 이 두 header의 include 순서는 상당히 중요하다.
// 컴파일러의 버그인지 모르겠지만, 순서가 서로 바뀌면 아래 static_assert가 실패하는 경우가 있기 때문이다.
#include "DecalPoolActor.h"
#include "ActorPool.h"
static_assert(TPointerIsConvertibleFromTo<APoolActor, AActor>::Value, "APoolActor* is not convertible to AActor*");


IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Saucewich, "Saucewich")

DEFINE_LOG_CATEGORY(LogSaucewich)

#if WITH_GAMELIFT

	#include "GameLiftServerSDK.h"
	DEFINE_LOG_CATEGORY(LogGameLift)

	FGameLiftServerSDKModule& USaucewich::GetGameLiftServerSDKModule()
	{
		static auto& Module = FModuleManager::GetModuleChecked<FGameLiftServerSDKModule>("GameLiftServerSDK");
		return Module;
	}

#endif

ADecalPoolActor* USaucewich::SpawnSauceDecal(const FHitResult& HitInfo, UMaterialInterface* const Material, const FLinearColor& Color,
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

AActorPool* USaucewich::GetActorPool(const UObject* const WorldContextObject)
{
	if (WorldContextObject)
		if (const auto World = WorldContextObject->GetWorld())
			if (const auto GI = World->GetGameInstance<USaucewichGameInstance>())
				return GI->GetActorPool();
	return nullptr;
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

int32 USaucewich::GetPlayerNameMinLen()
{
	return 1;
}

int32 USaucewich::GetPlayerNameMaxLen()
{
	return 16;
}

UUserSettings* USaucewich::GetUserSettings()
{
	static const auto UserSettings = NewObject<UUserSettings>(GetTransientPackage(), NAME_None, RF_MarkAsRootSet);
	return UserSettings;
}

int32 USaucewich::GetServerVersion()
{
	return 1;
}
