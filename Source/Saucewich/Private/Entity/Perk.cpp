// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Entity/Perk.h"
#include "Components/StaticMeshComponent.h"
#include "Names.h"

APerk::APerk()
	:Highlight{CreateDefaultSubobject<UStaticMeshComponent>(NAME("Highlight"))}
{
	Highlight->SetupAttachment(GetMesh());
	Highlight->SetRelativeScale3D_Direct(FVector{1.1f});
	Highlight->ForcedLodModel = 1;
	Highlight->SetCollisionProfileName(Names::NoCollision);
}
