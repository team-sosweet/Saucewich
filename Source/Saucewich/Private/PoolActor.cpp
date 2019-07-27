// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PoolActor.h"
#include "ActorPool.h"

void APoolActor::Release()
{
	if (!bActivated) return;
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	bActivated = false;
	Pool->Release(this);
	OnReleased();
}

void APoolActor::Activate()
{
	if (bActivated) return;
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	bActivated = true;
	OnActivated();
}
