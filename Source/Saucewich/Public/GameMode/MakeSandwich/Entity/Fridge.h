// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Fridge.generated.h"

UCLASS()
class SAUCEWICH_API AFridge final : public AStaticMeshActor
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	UPROPERTY(EditInstanceOnly)
	uint8 Team;
};
