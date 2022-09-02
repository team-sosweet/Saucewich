// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Entity/SauceMarker.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "GameMode/SaucewichGameMode.h"
#include "GameMode/SaucewichGameState.h"
#include "SaucewichInstance.h"
#include "Names.h"

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

	TArray<FHitResult> Hits;
	World->SweepMultiByChannel(
		Hits,
		Hit.ImpactPoint + Hit.ImpactNormal,
		Hit.ImpactPoint - Hit.ImpactNormal,
		Rot,
		GI->GetDecalTraceChannel(),
		FCollisionShape::MakeBox(FVector(Scale3D.X * 50.f, Scale3D.Y * 50.f, 0.f))
	);
	auto Offset = 0.f;
	for (auto&& H : Hits)
	{
		if ((H.ImpactNormal | Hit.ImpactNormal) > .99f)
		{
			Offset = 1.f - H.Distance;
			break;
		}
	}

	const auto Comp = Marker->TeamMarkers[Team].PickRand();
	Comp->AddInstance({Rot, Hit.ImpactPoint + Hit.ImpactNormal * (Offset + .01f), Scale3D}, true);
#endif
}

void ASauceMarker::Add(const AActor* const Owner, const uint8 Team, const FVector& Location, const float Scale)
{
#if !UE_SERVER
	const auto World = Owner->GetWorld();
	const auto MaxDist = 100.f * Scale;

	auto End = Location;
	End.Z -= MaxDist;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, Location, End, ECC_Visibility, Params))
	{
		Add(Team, Scale * (1 - Hit.Distance / MaxDist), Hit, Owner);
	}
#endif
}

void ASauceMarker::CleanupSauceMark(const UObject* const WorldContext, const FVector& Origin, const float Radius, const ECollisionChannel Channel)
{
	const auto World = WorldContext->GetWorld();
	const auto Shape = FCollisionShape::MakeSphere(Radius);
	
	TArray<FHitResult> Hits;
	if (!World->SweepMultiByChannel(Hits, Origin, Origin, FQuat::Identity, Channel, Shape)) return;
	
	TMap<UInstancedStaticMeshComponent*, TArray<int32>> Indices;
	for (const auto& Hit : Hits)
	{
		const auto Comp = Cast<UInstancedStaticMeshComponent>(Hit.GetComponent());
		if (Comp && !World->LineTraceTestByChannel(Origin, Hit.ImpactPoint, ECC_Visibility))
		{
			Indices.FindOrAdd(Comp).Add(Hit.Item);
		}
	}
	
	for (auto& Pair : Indices)
	{
		const auto Comp = Pair.Key;
		auto& Arr = Pair.Value;
		Arr.Sort();
		
		for (auto i=0; i<Arr.Num(); ++i)
		{
			const auto bSucceeded = Comp->RemoveInstance(Arr[i] - i);
			ensure(bSucceeded);
		}
	}
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
			->SetVectorParameterValue(Names::Color, Teams[i].Color);
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
