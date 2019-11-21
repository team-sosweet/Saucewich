// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"
#include "Translucentable.generated.h"

UINTERFACE(MinimalAPI)
class UTranslucentable : public UInterface
{
	GENERATED_BODY()
};

class SAUCEWICH_API ITranslucentable
{
	GENERATED_BODY()

public:
	virtual void BeTranslucent() = 0;
	virtual void BeOpaque() = 0;
};
