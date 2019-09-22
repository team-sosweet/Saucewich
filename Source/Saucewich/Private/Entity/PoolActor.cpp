// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "PoolActor.h"

#include "Engine/World.h"
#include "UnrealNetwork.h"

#include "Entity/ActorPool.h"
#include "SaucewichGameInstance.h"

void APoolActor::Release(const bool bForce)
{
	if (!IsValidLowLevel() || Activation == EActivation::Released && !bForce) return;
	
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetOwner(nullptr);
	
	Activation = EActivation::Released;

	if (!bReplicates || HasAuthority())
		GetPool()->Release(this);
	
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

AActorPool* APoolActor::GetPool() const
{
	if (const auto GI = GetWorld()->GetGameInstance<USaucewichGameInstance>())
		return GI->GetActorPool();
	return nullptr;
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
