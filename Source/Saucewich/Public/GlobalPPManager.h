// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GlobalPPManager.generated.h"

UCLASS()
class SAUCEWICH_API AGlobalPPManager : public AActor
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

private:
	void OnSettingChanged(uint8 Idx, bool bNewVal) const;

	class APostProcessVolume* PP;
};
