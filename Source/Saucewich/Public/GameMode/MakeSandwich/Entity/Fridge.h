// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Fridge.generated.h"

UCLASS()
class SAUCEWICH_API AFridge : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

public:
	AFridge();
	uint8 GetTeam() const { return Team; }

protected:
	void BeginPlay() override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 Team;
};
