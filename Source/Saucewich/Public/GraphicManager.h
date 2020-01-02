// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GraphicManager.generated.h"

enum class EGraphicOption : uint8;

UCLASS()
class SAUCEWICH_API AGraphicManager : public AActor
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

private:
	void OnOptionChanged(EGraphicOption Option, bool bEnabled) const;

	TArray<class AEmitter*> Emitters;
	class APostProcessVolume* PP;
};
