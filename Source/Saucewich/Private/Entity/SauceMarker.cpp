// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SauceMarker.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "SaucewichGameMode.h"
#include "SaucewichInstance.h"

UInstancedStaticMeshComponent* FSauceMarkers::PickRand() const
{
	return Comps[FMath::RandHelper(Comps.Num())];
}

void ASauceMarker::Add(const uint8 Team, const float Scale, const FHitResult& Hit, const UObject* const WorldContextObj)
{
#if !UE_SERVER
	const auto World = WorldContextObj->GetWorld();

	auto Rot = Hit.ImpactNormal.ToOrientationQuat();
	Rot *= FRotator{-90.f, 0.f, 0.f}.Quaternion();
	Rot *= FRotator{0.f, 360.f * FMath::FRand(), 0.f}.Quaternion();

	const auto Loc = Hit.ImpactPoint + Hit.ImpactNormal * .01f;

	const auto RandScale = [&]{return Scale * FMath::RandRange(.85f, 1.15f);};
	const FVector Scale3D{RandScale(), RandScale(), 1.f};

	const auto LineTraceTest = [&](const float AxisScale, auto&&... Dirs)
	{
		for (auto&& Dir : {Forward<decltype(Dirs)>(Dirs)...})
		{
			const auto Start = Loc + Rot.RotateVector(Dir) * (AxisScale * 50.f);
			if (!World->LineTraceTestByChannel(Start, Start - Hit.ImpactNormal, ECC_Visibility)) return false;
		}
		return true;
	};

	if (!LineTraceTest(Scale3D.X, FVector::ForwardVector, FVector::BackwardVector)) return;
	if (!LineTraceTest(Scale3D.Y, FVector::RightVector, FVector::LeftVector)) return;
	
	const auto Marker = USaucewichInstance::Get(World)->GetSauceMarker();
	const auto Comp = Marker->TeamMarkers[Team].PickRand();
	Comp->AddInstanceWorldSpace({Rot, Loc, Scale3D});
#endif
}

void ASauceMarker::BeginPlay()
{
	Super::BeginPlay();
	
#if !UE_SERVER
	auto&& Teams = ASaucewichGameMode::GetData(this).Teams;
	TeamMarkers.AddDefaulted(Teams.Num());
	for (auto i = 0; i < Teams.Num(); ++i)
	{
		for (auto&& Mat : Materials)
		{
			const auto Comp = CreateComp();
			TeamMarkers[i].Comps.Add(Comp);
			
			Comp->CreateDynamicMaterialInstance(0, Mat.LoadSynchronous())
			->SetVectorParameterValue(TEXT("Color"), Teams[i].Color);
		}
	}
#endif 
}
