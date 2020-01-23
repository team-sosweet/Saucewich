// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "UserSettings.h"
#include "Engine/Engine.h"
#include "Saucewich.h"
#include "SaucewichInstance.h"
#include "Kismet/BlueprintPlatformLibrary.h"

UUserSettings::UUserSettings()
	:bAutoFire{true}, bVibration{true}, bParticle{true}, bMusic{true}, bNotification{true}
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

void UUserSettings::SetMaxFPS(const float NewMaxFPS)
{
	MaxFPS = NewMaxFPS;
	CommitMaxFPS();
}

void UUserSettings::CommitMaxFPS() const
{
	if (MaxFPS > 0.f)
		GEngine->Exec(nullptr, *FString::Printf(TEXT("t.MaxFPS %f"), MaxFPS));
}

float UUserSettings::GetCorrectedSensitivity() const
{
	constexpr auto Correction = .25f;
	return Correction * RawSensitivity + Correction * .5f;
}

void UUserSettings::SetOutlineEnabled(const bool bEnabled)
{
	OnOptionChanged.ExecuteIfBound(EGraphicOption::Outline, bOutline = bEnabled);
}

void UUserSettings::SetHighlightEnabled(const bool bEnabled)
{
	OnOptionChanged.ExecuteIfBound(EGraphicOption::Highlight, bHighlight = bEnabled);
}

void UUserSettings::SetParticleEnabled(const bool bEnabled)
{
	OnOptionChanged.ExecuteIfBound(EGraphicOption::Particle, bParticle = bEnabled);
}

void UUserSettings::RegisterGraphicManager(FOnGraphicOptionChanged&& Callback)
{
	OnOptionChanged = MoveTemp(Callback);
	OnOptionChanged.Execute(EGraphicOption::Outline, bOutline);
	OnOptionChanged.Execute(EGraphicOption::Highlight, bHighlight);
	OnOptionChanged.Execute(EGraphicOption::Particle, bParticle);
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

	if (MaxFPS <= 0.f)
	{
		MaxFPS = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("t.MaxFPS"))->GetValueOnAnyThread();
	}
}
