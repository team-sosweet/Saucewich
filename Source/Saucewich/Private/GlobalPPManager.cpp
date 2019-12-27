// Copyright 2019 Othereum. All Rights Reserved.

#include "GlobalPPManager.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "UserSettings.h"

void AGlobalPPManager::BeginPlay()
{
	Super::BeginPlay();
	PP = *TActorIterator<APostProcessVolume>{GetWorld()};
	UUserSettings::Get(this)->RegisterPPManager(FOnPPSettingChanged::CreateUObject(this, &AGlobalPPManager::OnSettingChanged));
}

void AGlobalPPManager::OnSettingChanged(const uint8 Idx, const bool bNewVal) const
{
	PP->Settings.WeightedBlendables.Array[Idx].Weight = bNewVal;
}
