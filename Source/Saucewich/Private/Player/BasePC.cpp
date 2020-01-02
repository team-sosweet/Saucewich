// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

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
