// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PoolActor.generated.h"

UENUM()
enum class EActivation : uint8
{
	Invalid, Released, Activated
};

UCLASS()
class SAUCEWICH_API APoolActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Release(bool bForce = false);
	void Activate(bool bForce = false);
	bool IsActive() const { return Activation == EActivation::Activated; }
	void LifeSpanExpired() override { Release(); }

protected:
	void BeginPlay() override;
	
	virtual void OnReleased() {}
	virtual void OnActivated() {}

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnReleased"))
	void BP_OnReleased();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnActivated"))
	void BP_OnActivated();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_Activation();

	UPROPERTY(ReplicatedUsing=OnRep_Activation, Transient)
	EActivation Activation;
};
