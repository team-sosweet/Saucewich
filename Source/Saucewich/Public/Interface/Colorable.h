// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "Colorable.generated.h"

UINTERFACE(MinimalAPI)
class UColorable : public UInterface
{
	GENERATED_BODY()
};

class SAUCEWICH_API IColorable
{
	GENERATED_BODY()

public:
	virtual void SetColor(const FLinearColor& NewColor) = 0;
};
