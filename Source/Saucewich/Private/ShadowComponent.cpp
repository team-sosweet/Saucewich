// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "ShadowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SaucewichInstance.h"
#include "Names.h"

UShadowComponent::UShadowComponent()
{
#if !UE_SERVER
	PrimaryComponentTick.bCanEverTick = true;
	FSoftObjectPath MeshPath{TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'")};
	TSoftObjectPtr<UStaticMesh> Mesh{MeshPath};
	UStaticMeshComponent::SetStaticMesh(Mesh.LoadSynchronous());
	BodyInstance.SetCollisionProfileNameDeferred(Names::NoCollision);
#endif 
}


#if !UE_SERVER

void UShadowComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Offset.SetLocation(GetRelativeLocation());
	CreateDynamicMaterialInstance(0);
}

void UShadowComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* const ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto bShouldDraw = false;

	const auto World = GetWorld();
	const auto Transform = Offset * GetAttachParent()->GetComponentTransform();
	const auto Scale = GetRelativeScale3D();
	const auto MaxDist = Scale.X * 256.f;
	
	const auto Start = Transform.GetLocation();
	auto End = Start;
	End.Z -= MaxDist;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		const auto Rot = Hit.ImpactNormal.ToOrientationQuat() * FQuat{{-90.f, 0.f, 0.f}};
		auto MinDist = Hit.Distance;
		auto Num = 0;

		auto Offsets = {FVector::ForwardVector, FVector::BackwardVector, FVector::RightVector, FVector::LeftVector};
		for (auto&& Dir : Offsets)
		{
			const auto St = Start + Rot.RotateVector(Dir) * (Scale.X * 50.f);
			FHitResult H;
			if (World->LineTraceSingleByChannel(H, St, St - Hit.ImpactNormal * (Hit.Distance + 1.f), ECC_Visibility, Params)
				&& FMath::Abs(Hit.Distance - H.Distance) <= 1.f)
			{
				++Num;
				if (H.Distance < MinDist) MinDist = H.Distance;
			}
		}

		if (Num > 0)
		{
			const auto Size = Scale.X * 50.f;
			FHitResult H;
			const auto bOverlapped = World->SweepSingleByChannel(
				H, Start, Start - Hit.ImpactNormal * (Hit.Distance + 1.f), Rot,
				USaucewichInstance::Get(World)->GetDecalTraceChannel(),
				FCollisionShape::MakeBox(FVector{Size, Size, 0.f})
			);
			if (bOverlapped && H.Distance < MinDist) MinDist = H.Distance;

			const auto Mat = CastChecked<UMaterialInstanceDynamic>(GetMaterial(0));
			Mat->SetScalarParameterValue(NAME("Dist"), MinDist / MaxDist);
			Mat->SetScalarParameterValue(NAME("Opacity"), static_cast<float>(Num) / Offsets.size());
			SetWorldLocationAndRotation(Hit.ImpactPoint + Hit.ImpactNormal * (Hit.Distance - MinDist + .01f), Rot);
			bShouldDraw = true;
		}
	}

	SetVisibility(bShouldDraw);
}

#endif 
