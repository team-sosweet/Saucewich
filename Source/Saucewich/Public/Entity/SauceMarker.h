// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interface/Colorable.h"
#include "SauceMarker.generated.h"

class UInstancedStaticMeshComponent;

UCLASS(NotBlueprintable)
class SAUCEWICH_API ASauceMarker : public AActor, public IColorable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Add Sauce Mark", WorldContext=WorldContextObj))
	static void Add(uint8 Team, float Scale, const FHitResult& Hit, const UObject* WorldContextObj);

	ASauceMarker();

	void SetMaterial(UMaterialInterface* NewMaterial) const;
	void SetColor(const FLinearColor& NewColor) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UInstancedStaticMeshComponent* Meshes;
};
