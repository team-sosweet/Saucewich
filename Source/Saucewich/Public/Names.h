// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "UObject/NameTypes.h"

#define NAME(n) []()->auto&{static const FName N{TEXT(n)};return N;}()

namespace Names
{
	extern const FName Color;
	extern const FName TeamColor;
	extern const FName Mesh;
	extern const FName Movement;
	extern const FName Projectile;
	extern const FName Collision;
	extern const FName Shadow;
	extern const FName Pickup;
	extern const FName NoCollision;
	extern const FName FirePSC;
	extern const FName Muzzle;
	extern const FName RPM;
	extern const FName WeaponComponent;
	extern const FName SpringArm;
	extern const FName Camera;
	extern const FName Weapon;
	extern const FName Win;
	extern const FName Draw;
	extern const FName Lose;
}
