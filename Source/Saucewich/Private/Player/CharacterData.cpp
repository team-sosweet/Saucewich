// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "CharacterData.h"

#include "TranslMatData.h"

UMaterialInterface* UCharacterData::GetTranslMat(const uint8 Idx, const UMaterialInterface* const Mat) const
{
	if (const auto Found = TranslMatOverride.Find(Idx))
		return *Found;

	return SharedTranslMat->Map.FindRef(Mat).LoadSynchronous();
}
