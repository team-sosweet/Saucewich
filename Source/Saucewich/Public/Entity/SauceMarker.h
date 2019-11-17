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

	UInstancedStaticMeshComponent* Pick() const { return Comps[FMath::RandHelper(Comps.Num())]; }
};

UCLASS(NotPlaceable)
class SAUCEWICH_API ASauceMarker : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Add Sauce Mark", WorldContext=WorldContextObj))
	static void Add(uint8 Team, float Scale, const FHitResult& Hit, const UObject* WorldContextObj);

protected:
	void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(Transient)
	TArray<FSauceMarkers> TeamMarkers;
};
