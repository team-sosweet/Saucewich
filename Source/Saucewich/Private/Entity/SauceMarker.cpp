// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SauceMarker.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SaucewichGameState.h"
#include "Saucewich.h"
#include "ConstructorHelpers.h"

void ASauceMarker::Add(const uint8 Team, const float Scale, const FHitResult& Hit, const UObject* const WorldContextObj)
{
	const auto World = WorldContextObj->GetWorld();
	const auto Marker = ASaucewichGameState::GetSauceMarker(Team, World);

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

	Marker->Meshes->AddInstanceWorldSpace({Rot, Loc, Scale3D});
}

ASauceMarker::ASauceMarker()
	:Meshes{CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Meshes"))}
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane{TEXT("/Engine/BasicShapes/Plane")};
	
	RootComponent = Meshes;
	Meshes->SetStaticMesh(Plane.Object);
}

void ASauceMarker::SetMaterial(UMaterialInterface* const NewMaterial) const
{
	Meshes->CreateDynamicMaterialInstance(0, NewMaterial);
}

void ASauceMarker::SetColor(const FLinearColor& NewColor)
{
	CastChecked<UMaterialInstanceDynamic>(Meshes->GetMaterial(0))->SetVectorParameterValue(TEXT("Color"), NewColor);
}
