// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "DecalPoolActor.generated.h"

UCLASS()
class SAUCEWICH_API ADecalPoolActor final : public APoolActor
{
	GENERATED_BODY()

public:
	ADecalPoolActor();

private:
	UPROPERTY(VisibleAnywhere)
	UDecalComponent* Decal;
};
