// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "SauceMarker.generated.h"

class UInstancedStaticMeshComponent;

USTRUCT()
struct FSauceMarkers
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	TArray<UInstancedStaticMeshComponent*> Comps;

	UInstancedStaticMeshComponent* PickRand() const;
};

UCLASS(NotPlaceable)
class SAUCEWICH_API ASauceMarker : public AActor
{
	GENERATED_BODY()
	
public:
	static void Add(uint8 Team, float Scale, const FHitResult& Hit, const AActor* Ignore);

protected:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	UInstancedStaticMeshComponent* CreateComp();

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(Transient)
	TArray<FSauceMarkers> TeamMarkers;
};
