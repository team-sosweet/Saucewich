// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Entity/PoolActor.h"
#include "Interface/Colorable.h"
#include "DecalPoolActor.generated.h"

UCLASS()
class SAUCEWICH_API ADecalPoolActor final : public APoolActor, public IColorable
{
	GENERATED_BODY()

public:
	ADecalPoolActor();
	UDecalComponent* GetDecal() const { return Decal; }

	UFUNCTION(BlueprintCallable)
	void SetColor(const FLinearColor& NewColor) override;
	
protected:
	void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UDecalComponent* Decal;
};
