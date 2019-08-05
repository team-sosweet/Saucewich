// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PoolActor.generated.h"

UCLASS()
class SAUCEWICH_API APoolActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Release(bool bForce = false);
	void Activate(bool bForce = false);
	bool IsActive() const { return bActivated; }
	void LifeSpanExpired() override { Release(); }
	class AActorPool* GetPool() const { return Pool; }

protected:
	virtual void OnReleased() {}
	virtual void OnActivated() {}

	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_Activated();

	AActorPool* Pool;

	UPROPERTY(ReplicatedUsing=OnRep_Activated, Transient)
	uint8 bActivated : 1;
};
