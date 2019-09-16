// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "DecalPoolActor.h"

#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

ADecalPoolActor::ADecalPoolActor()
	:Decal{CreateDefaultSubobject<UDecalComponent>("Decal")}
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = Decal;
	Decal->bAbsoluteScale = true;
}

void ADecalPoolActor::SetColor(const FLinearColor& NewColor)
{
	auto Mat = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial());
	if (!Mat) Mat = Decal->CreateDynamicMaterialInstance();
	if (Mat)
	{
		Mat->SetVectorParameterValue("Color", NewColor);
		Mat->SetScalarParameterValue("Opacity", 1);
	}
}

void ADecalPoolActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (InitialLifeSpan > 0)
	{
		const auto Alpha = GetLifeSpan() / InitialLifeSpan * 3;
		if (Alpha > 0 && Alpha < 1)
		{
			if (const auto Mat = Cast<UMaterialInstanceDynamic>(Decal->GetDecalMaterial()))
			{
				Mat->SetScalarParameterValue("Opacity", Alpha);
			}
		}
	}
}
