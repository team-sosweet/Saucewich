// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "UserSettings.h"
#include "Saucewich.h"

UUserSettings* UUserSettings::Get()
{
	static const auto UserSettings = NewObject<UUserSettings>(GetTransientPackage(), NAME_None, RF_MarkAsRootSet);
	return UserSettings;
}

ENameValidity UUserSettings::SetPlayerName(const FString& NewPlayerName)
{
	const auto Validity = USaucewich::IsValidPlayerName(NewPlayerName);
	if (Validity == ENameValidity::Valid)
	{
		PlayerName = NewPlayerName;
		Save();
	}
	return Validity;
}

float UUserSettings::GetSensitivity() const
{
	constexpr auto Correction = 1.f;
	return Correction * Sensitivity + Correction * .5f;
}

void UUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
	{
		PlayerName = TEXT("유저");
		Save();
	}
}
