// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "RepDat.generated.h"

UCLASS()
class SAUCEWICH_API ARepDat : public AActor
{
	GENERATED_BODY()

public:
	ARepDat();

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
