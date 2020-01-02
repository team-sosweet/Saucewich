// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "ShadowComponent.generated.h"

UCLASS()
class SAUCEWICH_API UShadowComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UShadowComponent();

#if !UE_SERVER
		
protected:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	FTransform Offset;

#endif
};
