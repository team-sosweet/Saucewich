// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Interface/Colorable.h"
#include "DecalPoolActor.generated.h"

UCLASS()
class SAUCEWICH_API ADecalPoolActor : public APoolActor, public IColorable
{
	GENERATED_BODY()

public:
	ADecalPoolActor();

	UFUNCTION(BlueprintCallable)
	void SetColor(const FLinearColor& NewColor) override;

	void SetDecalMaterial(UMaterialInterface* NewMaterial) const;
	void SetDecalSize(const FVector& NewSize) const;
	
protected:
	void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UDecalComponent* Decal;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collision;
};
