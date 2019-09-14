// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "PoolActor.generated.h"

/**
 * 재활용 가능한 액터입니다. 자주 생성/소멸되는 액터에 좋습니다.
 */
UCLASS(Abstract)
class SAUCEWICH_API APoolActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Release(bool bForce = false);
	void Activate(bool bForce = false);
	bool IsActive() const { return bActivated; }
	void LifeSpanExpired() override { Release(); }
	class AActorPool* GetPool() const;

protected:
	virtual void OnReleased() {}
	virtual void OnActivated() {}

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnReleased"))
	void BP_OnReleased();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnActivated"))
	void BP_OnActivated();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_Activated();

	UPROPERTY(ReplicatedUsing=OnRep_Activated, Transient)
	uint8 bActivated : 1;
};
