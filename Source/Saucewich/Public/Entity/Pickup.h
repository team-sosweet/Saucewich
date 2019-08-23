// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Pickup.generated.h"

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

protected:
	void Tick(float DeltaSeconds) override;

	void OnReleased() override;
	void OnActivated() override;

private:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetLocation(FVector Location);
	
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
