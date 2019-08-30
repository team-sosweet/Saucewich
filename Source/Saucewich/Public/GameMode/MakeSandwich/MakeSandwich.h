// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "Online/SaucewichGameMode.h"
#include "MakeSandwich.generated.h"

UCLASS()
class SAUCEWICH_API AMakeSandwich : public ASaucewichGameMode
{
	GENERATED_BODY()

protected:
	bool ReadyToEndMatch_Implementation() override;
};
