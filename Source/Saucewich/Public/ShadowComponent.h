// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Translucentable.h"
#include "ShadowComponent.generated.h"

UCLASS()
class SAUCEWICH_API UShadowComponent : public UStaticMeshComponent, public ITranslucentable
{
	GENERATED_BODY()
	
public:
	UShadowComponent();

	void BeTranslucent() override;
	void BeOpaque() override;
	
#if !UE_SERVER
		
protected:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	FTransform Offset;
	uint8 bTranslucent : 1;
	
#endif
};
