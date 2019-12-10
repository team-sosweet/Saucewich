// Copyright 2019 Othereum. All Rights Reserved.

#include "Weapon/WeaponSharedData.h"

#include "TranslMatData.h"

UMaterialInterface* UWeaponSharedData::GetTranslMat(const UMaterialInterface* const OrigMat) const
{
	return SharedTranslMat->Map.FindRef(OrigMat).LoadSynchronous();
}
