// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "SauceMarker.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "SaucewichGameMode.h"
#include "SaucewichInstance.h"
#include "SaucewichGameState.h"

UInstancedStaticMeshComponent* FSauceMarkers::PickRand() const
{
	return Comps[FMath::RandHelper(Comps.Num())];
}

void ASauceMarker::Add(const uint8 Team, const float Scale, const FHitResult& Hit, const AActor* const Ignore)
{
#if !UE_SERVER
	const auto World = Ignore->GetWorld();

	auto Rot = Hit.ImpactNormal.ToOrientationQuat();
	Rot *= FRotator{-90.f, 0.f, 0.f}.Quaternion();
	Rot *= FRotator{0.f, 360.f * FMath::FRand(), 0.f}.Quaternion();

	const auto RandScale = [&]{return Scale * FMath::RandRange(.85f, 1.15f);};
	const FVector Scale3D{RandScale(), RandScale(), 1.f};

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Ignore);

	const auto LineTraceTest = [&](const float AxisScale, auto&&... Dirs)
	{
		for (auto&& Dir : {Forward<decltype(Dirs)>(Dirs)...})
		{
			const auto Offset = Rot.RotateVector(Dir) * (AxisScale * 50.f);
			const auto Loc = Hit.ImpactPoint + Offset;
			if (!World->LineTraceTestByChannel(Loc + Hit.ImpactNormal, Loc - Hit.ImpactNormal, ECC_Visibility, Params))
				return false;
		}
		return true;
	};

	if (!LineTraceTest(Scale3D.X, FVector::ForwardVector, FVector::BackwardVector)) return;
	if (!LineTraceTest(Scale3D.Y, FVector::RightVector, FVector::LeftVector)) return;

	const auto GI = USaucewichInstance::Get(World);
	const auto Marker = GI->GetSauceMarker();

	FHitResult H;
	const auto bOverlapped = World->SweepSingleByChannel(
		H,
		Hit.ImpactPoint + Hit.ImpactNormal,
		Hit.ImpactPoint - Hit.ImpactNormal,
		Rot,
		GI->GetDecalTraceChannel(),
		FCollisionShape::MakeBox({Scale3D.X * 50.f, Scale3D.Y * 50.f, 0.f})
	);
	const auto Offset = bOverlapped ? 1.01f - H.Distance : .01f;

	const auto Comp = Marker->TeamMarkers[Team].PickRand();
	Comp->AddInstanceWorldSpace({Rot, Hit.ImpactPoint + Hit.ImpactNormal * Offset, Scale3D});
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

	CastChecked<ASaucewichGameState>(GetWorld()->GetGameState())->OnCleanup.AddUObject(this, &ASauceMarker::Cleanup);
#endif 
}

void ASauceMarker::Cleanup()
{
	for (auto&& Markers : TeamMarkers)
		for (auto&& Comp : Markers.Comps)
			Comp->ClearInstances();
}
