// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PoolActor.generated.h"

UCLASS()
class SAUCEWICH_API APoolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	void Release();
	void Activate();
	void LifeSpanExpired() override { Release(); }
	void SetPool(class AActorPool* NewPool) { Pool = NewPool; }

protected:
	virtual void OnReleased() {}
	virtual void OnActivated() {}

private:
	AActorPool* Pool;
	uint8 bActivated : 1;
};
