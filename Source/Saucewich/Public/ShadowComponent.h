// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Translucentable.h"
#include "ShadowComponent.generated.h"

/**
 * 아래에 모바일용 저사양 동그란 그림자를 띄우는 컴포넌트 입니다.
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class SAUCEWICH_API UShadowComponent final : public UStaticMeshComponent, public ITranslucentable
{
	GENERATED_BODY()

public:
	UShadowComponent();

	void BeTranslucent() override { bTranslucent = true; }
	void BeOpaque() override { bTranslucent = false; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Darkness = .9f;
	
protected:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	class UMaterialInstanceDynamic* Material;
	FTransform Offset;
	uint8 bTranslucent : 1;
};
