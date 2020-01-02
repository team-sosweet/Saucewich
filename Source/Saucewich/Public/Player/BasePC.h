// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "BasePC.generated.h"

UCLASS()
class SAUCEWICH_API ABasePC : public APlayerController
{
	GENERATED_BODY()

protected:
	void SetupInputComponent() override;

private:
	void OpenMenu();
};
