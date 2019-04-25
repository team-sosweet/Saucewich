// Copyright (c) 2019, Team Sosweet. All rights reserved.

#pragma once

UENUM(BlueprintType)
enum class EWeaponGripType : uint8
{
	Rifle, Pistol
};

UENUM()
enum class EWeaponPosition : uint8
{
	Primary, Secondary,
	_MAX UMETA(Hidden)
};
