// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "UserSettings.generated.h"

enum class ENameValidity : uint8;

UCLASS(Config=GameUserSettings)
class SAUCEWICH_API UUserSettings : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, DisplayName="Get User Settings")
	static UUserSettings* Get();
	
	UFUNCTION(BlueprintCallable)
	void Save() { SaveConfig(); }
	
	UFUNCTION(BlueprintCallable)
	ENameValidity SetPlayerName(const FString& NewPlayerName);
	const FString& GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	float GetCorrectedSensitivity() const;

	
	UPROPERTY(Config, BlueprintReadWrite)
	float RawSensitivity = .5;

	UPROPERTY(Config, BlueprintReadWrite)
	uint8 bAutoFire : 1;

protected:
	void PostInitProperties() override;

private:
	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString PlayerName;
};
