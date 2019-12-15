// Copyright 2019 Othereum. All Rights Reserved.

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
	static void Add(const AActor* Owner, uint8 Team, const FVector& Location, float Scale = 1.f);

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContext))
	static void CleanupSauceMark(const UObject* WorldContext, const FVector& Origin, float Radius, ECollisionChannel Channel);

protected:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	UInstancedStaticMeshComponent* CreateComp();

private:
	void Cleanup();
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(Transient)
	TArray<FSauceMarkers> TeamMarkers;
};
