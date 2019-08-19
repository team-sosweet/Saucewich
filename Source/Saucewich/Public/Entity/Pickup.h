// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Pickup.generated.h"

/**
 * 캐릭터가 획득할 수 있는 아이템
 */
UCLASS()
class SAUCEWICH_API APickup : public APoolActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:
	APickup();

protected:
	void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	float BounceScale;

	UPROPERTY(EditAnywhere)
	float BounceSpeed;

	UPROPERTY(EditAnywhere)
	float RotateSpeed;
};
