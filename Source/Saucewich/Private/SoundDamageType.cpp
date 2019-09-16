// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SoundDamageType.h"

USoundBase* USoundDamageType::GetSound() const
{
	return Sounds.Num() > 0 ? Sounds[FMath::RandHelper(Sounds.Num())].LoadSynchronous() : nullptr;
}
