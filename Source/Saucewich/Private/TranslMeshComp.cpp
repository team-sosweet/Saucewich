// Copyright 2019 Othereum. All Rights Reserved.

#include "TranslMeshComp.h"

void UTranslMeshComp::BeTransl()
{
	if (bTransl) return;
	const auto Num = FMath::Min(GetNumMaterials(), TranslMats.Num());
	for (auto i = 0; i < Num; ++i) if (TranslMats[i]) SetMaterial(i, TranslMats[i]);
}

void UTranslMeshComp::BeOpaque()
{
	if (!bTransl) return;
	const auto DefMats = GetDefault<UTranslMeshComp>(GetClass())->GetMaterials();
	for (auto i = 0; i < DefMats.Num(); ++i) SetMaterial(i, DefMats[i]);
}
