// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "UserSettings.generated.h"

enum class ENameValidity : uint8;
enum class EGraphicOption : uint8
{
	Outline, Highlight, Particle = 255
};

DECLARE_EVENT(UUserSettings, FOnNotificationDisabled)

UCLASS(Config=UserSettings)
class SAUCEWICH_API UUserSettings : public UObject
{
	GENERATED_BODY()

public:
	UUserSettings();
	
	UFUNCTION(BlueprintPure, DisplayName="Get User Settings", meta=(WorldContext=W))
	static UUserSettings* Get(const UObject* W);
	
	UFUNCTION(BlueprintCallable)
	void Save() { SaveConfig(); }
	
	UFUNCTION(BlueprintCallable)
	ENameValidity SetPlayerName(const FString& NewPlayerName);
	const FString& GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	float GetCorrectedSensitivity() const;

	UFUNCTION(BlueprintCallable)
	void SetNotificationEnabled(bool bEnabled);
	bool IsNotificationEnabled() const { return bNotification; }


	UPROPERTY(Config, BlueprintReadWrite)
	float RawSensitivity = .5;

	UPROPERTY(Config, BlueprintReadWrite)
	uint8 bAutoFire : 1;

	UPROPERTY(Config, BlueprintReadWrite)
	uint8 bVibration : 1;

	FOnNotificationDisabled OnNotificationDisabled;

protected:
	void PostInitProperties() override;

private:
	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString PlayerName;

	UPROPERTY(Config, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	uint8 bMusic : 1;

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bNotification : 1;
};
