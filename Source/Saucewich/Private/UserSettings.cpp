// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "UserSettings.h"
#include "SaucewichLibrary.h"

bool UUserSettings::SetPlayerName(const FString& NewPlayerName)
{
	if (!USaucewichLibrary::IsValidPlayerName(NewPlayerName))
		return false;
	
	PlayerName = NewPlayerName;
	SaveConfig();
	return true;
}

void UUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (!USaucewichLibrary::IsValidPlayerName(PlayerName))
	{
		PlayerName = "Player";
		SaveConfig();
	}
}
