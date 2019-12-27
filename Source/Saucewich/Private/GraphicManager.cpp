// Copyright 2019 Othereum. All Rights Reserved.

#include "GraphicManager.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "Particles/Emitter.h"
#include "UserSettings.h"

void AGraphicManager::BeginPlay()
{
	Super::BeginPlay();
	const auto World = GetWorld();
	
	PP = *TActorIterator<APostProcessVolume>{World};
	
	for (const auto Emitter : TActorRange<AEmitter>{World})
		Emitters.Add(Emitter);
	
	UUserSettings::Get(this)->RegisterGraphicManager(FOnGraphicOptionChanged::CreateUObject(this, &AGraphicManager::OnOptionChanged));
}

void AGraphicManager::OnOptionChanged(const EGraphicOption Option, const bool bEnabled) const
{
	switch (Option)
	{
	case EGraphicOption::Particle:
		for (const auto Emitter : Emitters)
		{
			if (bEnabled) Emitter->Activate();
			else Emitter->Deactivate();
		}
		break;
		
	default:
		PP->Settings.WeightedBlendables.Array[static_cast<int32>(Option)].Weight = bEnabled;
	}
}
