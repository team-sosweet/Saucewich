// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "TranslMeshComp.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class UTranslMeshComp : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	void BeTransl();
	void BeOpaque();
	bool IsTransl() const { return bTransl; }

private:
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UMaterialInterface>> TranslMats;

	uint8 bTransl : 1;
};
