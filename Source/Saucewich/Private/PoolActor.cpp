// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PoolActor.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"
#include "ActorPool.h"
#include "SaucewichGameInstance.h"

void APoolActor::Release(const bool bForce)
{
	if (!bActivated && !bForce) return;
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	bActivated = false;
	Pool->Release(this);
	OnReleased();
}

void APoolActor::Activate(const bool bForce)
{
	if (bActivated && !bForce) return;
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	SetLifeSpan(InitialLifeSpan);
	bActivated = true;
	OnActivated();
}

void APoolActor::BeginPlay()
{
	Super::BeginPlay();

	if (const auto World = GetWorld())
		if (const auto GI = World->GetGameInstance<USaucewichGameInstance>())
			Pool = GI->GetActorPool();
}

void APoolActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolActor, bActivated);
}

void APoolActor::OnRep_Activated()
{
	if (bActivated) Activate(true);
	else Release(true);
}
