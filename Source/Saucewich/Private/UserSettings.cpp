// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "UserSettings.h"
#include "Engine/Engine.h"
#include "Saucewich.h"
#include "SaucewichInstance.h"
#include "Kismet/BlueprintPlatformLibrary.h"

UUserSettings::UUserSettings()
	:bAutoFire{true}, bVibration{true}, bMusic{true}, bNotification{true}
{
}

UUserSettings* UUserSettings::Get(const UObject* const W)
{
	return USaucewichInstance::Get(W)->GetUserSettings();
}

ENameValidity UUserSettings::SetPlayerName(const FString& NewPlayerName)
{
	const auto Validity = USaucewich::IsValidPlayerName(NewPlayerName);
	if (Validity == ENameValidity::Valid)
	{
		PlayerName = NewPlayerName;
	}
	return Validity;
}

float UUserSettings::GetCorrectedSensitivity() const
{
	constexpr auto Correction = .25f;
	return Correction * RawSensitivity + Correction * .5f;
}

void UUserSettings::SetNotificationEnabled(const bool bEnabled)
{
	if (!bEnabled)
	{
		UBlueprintPlatformLibrary::ClearAllLocalNotifications();
		OnNotificationDisabled.Broadcast();
	}
	bNotification = bEnabled;
}

void UUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (USaucewich::IsValidPlayerName(PlayerName) != ENameValidity::Valid)
	{
		PlayerName = TEXT("Player");
		PlayerName.AppendInt(FMath::RandRange(100, 999));
	}
}
