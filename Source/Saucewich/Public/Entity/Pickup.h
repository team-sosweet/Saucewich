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

private:
	UPROPERTY(EditAnywhere)
	float BounceScale;

	UPROPERTY(EditAnywhere)
	float BounceSpeed;

	UPROPERTY(EditAnywhere)
	float RotateSpeed;
};
