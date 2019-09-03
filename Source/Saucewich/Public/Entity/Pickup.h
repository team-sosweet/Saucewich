// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Pickup.generated.h"

class ATpsCharacter;

UCLASS()
class SAUCEWICH_API APickup : public APoolActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	class UShadowComponent* Shadow;

public:
	APickup();
	uint8 bSpawnedFromSpawner : 1;

protected:
	void Tick(float DeltaSeconds) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	void OnActivated() override;
	void OnReleased() override;

	virtual void BePickedUp(ATpsCharacter* By);
	virtual void StartPickUp(ATpsCharacter* By);
	virtual void CancelPickUp(ATpsCharacter* By);
	virtual bool CanPickedUp(const ATpsCharacter* By) const { return true; }
	virtual bool CanEverPickedUp(const ATpsCharacter* By) const { return true; }
	
private:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetLocation(FVector Location);

	ATpsCharacter* PickingChar;
	float PickingTimer;

	// 재료를 획득하는데 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	float PickupTime = 1;
	
	UPROPERTY(EditAnywhere)
	float BounceScale = 10;

	UPROPERTY(EditAnywhere)
	float BounceSpeed = 5;

	UPROPERTY(EditAnywhere)
	float RotateSpeed = 100;

	// Pickup끼리 겹쳐있을 때 서로 밀어내는 힘의 세기입니다.
	UPROPERTY(EditAnywhere)
	float PushStrength = 1000;
};
