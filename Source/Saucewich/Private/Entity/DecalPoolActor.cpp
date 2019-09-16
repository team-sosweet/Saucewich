// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "DecalPoolActor.h"

#include "Components/DecalComponent.h"

ADecalPoolActor::ADecalPoolActor()
	:Decal{CreateDefaultSubobject<UDecalComponent>("Decal")}
{
	RootComponent = Decal;
}
