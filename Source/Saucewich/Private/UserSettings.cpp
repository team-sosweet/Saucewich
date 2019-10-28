// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "UserSettings.h"
#include "Saucewich.h"

ENameValidity UUserSettings::SetPlayerName(const FString& NewPlayerName)
{
	const auto Validity = USaucewich::IsValidPlayerName(NewPlayerName);
	if (Validity == ENameValidity::Valid)
	{
		PlayerName = NewPlayerName;
		SaveConfig();
	}
	return Validity;
}

void UUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
	{
		PlayerName = TEXT("유저");
		SaveConfig();
	}
}
