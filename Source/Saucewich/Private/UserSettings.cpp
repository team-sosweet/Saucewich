// Copyright 2019 Othereum. All Rights Reserved.

#include "UserSettings.h"
#include "Saucewich.h"
#include "Engine/Engine.h"

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
	}
	return Validity;
}

void UUserSettings::SetMaxFPS(const float NewMaxFPS)
{
	MaxFPS = NewMaxFPS;
	CommitMaxFPS();
}

void UUserSettings::CommitMaxFPS() const
{
	GEngine->Exec(nullptr, *FString::Printf(TEXT("t.MaxFPS %f"), MaxFPS));
}

float UUserSettings::GetCorrectedSensitivity() const
{
	constexpr auto Correction = .25f;
	return Correction * RawSensitivity + Correction * .5f;
}

void UUserSettings::SetOutlineEnabled(const bool bEnabled)
{
	bOutline = bEnabled;
	OnPPSettingChanged.ExecuteIfBound(0, bEnabled);
}

void UUserSettings::SetHighlightEnabled(const bool bEnabled)
{
	bHighlight = bEnabled;
	OnPPSettingChanged.ExecuteIfBound(1, bEnabled);
}

void UUserSettings::RegisterPPManager(FOnPPSettingChanged&& Callback)
{
	OnPPSettingChanged = MoveTemp(Callback);
	OnPPSettingChanged.Execute(0, bOutline);
	OnPPSettingChanged.Execute(1, bHighlight);
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
