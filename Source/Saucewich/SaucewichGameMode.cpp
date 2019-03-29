// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SaucewichGameMode.h"
#include "SaucewichCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASaucewichGameMode::ASaucewichGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
