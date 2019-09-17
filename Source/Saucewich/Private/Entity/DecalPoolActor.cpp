// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "DecalPoolActor.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

ADecalPoolActor::ADecalPoolActor()
	:Decal{CreateDefaultSubobject<UDecalComponent>("Decal")},
	Collision{CreateDefaultSubobject<UBoxComponent>("Collision")}
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = Decal;
	Decal->bAbsoluteScale = true;

	Collision->SetupAttachment(Decal);
	Collision->SetCollisionProfileName("Decal");
}

void ADecalPoolActor::SetColor(const FLinearColor& NewColor)
{
	if (const auto Mat = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial()))
	{
		Mat->SetVectorParameterValue("Color", NewColor);
	}
}

void ADecalPoolActor::SetDecalMaterial(UMaterialInterface* const NewMaterial) const
{
	if (!NewMaterial) return;
	
	if (const auto OldMat = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial()))
	{
		if (OldMat->Parent == NewMaterial)
		{
			OldMat->SetScalarParameterValue("Opacity", 1);
			return;
		}
	}
	
	const auto Mat = UMaterialInstanceDynamic::Create(NewMaterial, Decal);
	Decal->SetDecalMaterial(Mat ? Mat : NewMaterial);
}

void ADecalPoolActor::SetDecalSize(const FVector& NewSize) const
{
	Decal->DecalSize = NewSize;
	Collision->SetBoxExtent(NewSize);
}

void ADecalPoolActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (InitialLifeSpan > 0)
	{
		if (const auto Mat = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial()))
		{
			Mat->SetScalarParameterValue("Opacity", GetLifeSpan() / InitialLifeSpan);
		}
	}
}
