// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SauceMarker.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "SaucewichGameMode.h"
#include "SaucewichInstance.h"

void ASauceMarker::Add(const uint8 Team, const float Scale, const FHitResult& Hit, const UObject* const WorldContextObj)
{
	const auto World = WorldContextObj->GetWorld();

	auto Rot = Hit.ImpactNormal.ToOrientationQuat();
	Rot *= FRotator{-90.f, 0.f, 0.f}.Quaternion();
	Rot *= FRotator{0.f, 360.f * FMath::FRand(), 0.f}.Quaternion();
	
	auto Loc = Hit.ImpactNormal;
	Loc *= .01f;
	Loc += Hit.ImpactPoint;

	FVector Scale3D{Scale, Scale, 1.f};
	const auto Rand = []{constexpr auto Factor = .15f; return 1.f + FMath::FRandRange(-Factor, Factor);};
	Scale3D.X *= Rand();
	Scale3D.Y *= Rand();

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

	World->GetGameInstanceChecked<USaucewichInstance>()->GetSauceMarker()
	->TeamMarkers[Team - 1].Pick()->AddInstanceWorldSpace({Rot, Loc, Scale3D});
}

void ASauceMarker::BeginPlay()
{
	Super::BeginPlay();
	
	const auto Mesh = TSoftObjectPtr<UStaticMesh>{{TEXT("/Engine/BasicShapes/Plane")}}.LoadSynchronous();
	auto&& Teams = CastChecked<ASaucewichGameMode>(GetWorld()->GetGameState()->GetDefaultGameMode())->GetData().Teams;
	TeamMarkers.AddDefaulted(Teams.Num());
	for (auto i = 0; i < Teams.Num(); ++i)
	{
		for (auto&& Mat : Materials)
		{
			const auto Comp = NewObject<UInstancedStaticMeshComponent>(this);
			Comp->SetStaticMesh(Mesh);
			Comp->CreateDynamicMaterialInstance(0, Mat.LoadSynchronous())
			->SetVectorParameterValue(TEXT("Color"), Teams[i].Color);
			Comp->RegisterComponentWithWorld(GetWorld());
			TeamMarkers[i].Comps.Add(Comp);
		}
	}
}
