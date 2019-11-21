// Copyright 2019 Seokjin Lee. All Rights Reserved.

#include "WeaponSharedData.h"

#include "TranslMatData.h"

UMaterialInterface* UWeaponSharedData::GetTranslMat(const UMaterialInterface* const OrigMat) const
{
	return SharedTranslMat->Map.FindRef(OrigMat).LoadSynchronous();
}
