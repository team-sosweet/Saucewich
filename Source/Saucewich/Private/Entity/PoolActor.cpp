// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PoolActor.h"

#include "Engine/World.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "SaucewichGameState.h"

void APoolActor::Release(const bool bForce)
{
	check(IsValidLowLevel());
	if (Activation == EActivation::Released && !bForce) return;
	
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetOwner(nullptr);
	
	Activation = EActivation::Released;

	if (!bReplicates || HasAuthority())
		AActorPool::Get(this)->Release(this);
	
	OnReleased();
	BP_OnReleased();
}

void APoolActor::Activate(const bool bForce)
{
	if (IsActive() && !bForce) return;
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
	SetLifeSpan(InitialLifeSpan);
	Activation = EActivation::Activated;
	OnActivated();
	BP_OnActivated();
}

void APoolActor::BeginPlay()
{
	Super::BeginPlay();
	CastChecked<ASaucewichGameState>(GetWorld()->GetGameState())->OnCleanup.AddUObject(this, &APoolActor::Release, false);
}

void APoolActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolActor, Activation);
}

void APoolActor::OnRep_Activation()
{
	if (IsActive()) Activate(true);
	else Release(true);
}
