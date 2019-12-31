// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/CharacterData.h"
#include "TranslMatData.h"

UMaterialInterface* UCharacterData::GetTranslMat(const uint8 Idx, const UMaterialInterface* const Mat) const
{
	if (const auto Found = TranslMatOverride.Find(Idx))
		return *Found;

	const auto Found = SharedTranslMat->Map.Find(Mat);
	if (ensure(Found)) return Found->LoadSynchronous();

	return nullptr;
}
