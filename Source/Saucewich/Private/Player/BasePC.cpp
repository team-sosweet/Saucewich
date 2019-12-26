// Copyright 2019 Othereum. All Rights Reserved.

#include "Player/BasePC.h"
#include "Names.h"
#include "Player/BaseHUD.h"

void ABasePC::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(NAME("Menu"), IE_Pressed, this, &ABasePC::OpenMenu);
}

void ABasePC::OpenMenu()
{
	CastChecked<ABaseHUD>(GetHUD())->OpenMenu();
}
