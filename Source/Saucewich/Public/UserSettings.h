// Copyright 2019 Othereum. All Rights Reserved.

#pragma once

#include "UserSettings.generated.h"

enum class ENameValidity : uint8;

DECLARE_DELEGATE_TwoParams(FOnPPSettingChanged, uint8, bool)

UCLASS(Config=GameUserSettings)
class SAUCEWICH_API UUserSettings : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, DisplayName="Get User Settings", meta=(WorldContext=W))
	static UUserSettings* Get(const UObject* W);
	
	UFUNCTION(BlueprintCallable)
	void Save() { SaveConfig(); }
	
	UFUNCTION(BlueprintCallable)
	ENameValidity SetPlayerName(const FString& NewPlayerName);
	const FString& GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	void SetMaxFPS(float NewMaxFPS);
	void CommitMaxFPS() const;
	float GetMaxFPS() const { return MaxFPS; }

	UFUNCTION(BlueprintCallable)
	float GetCorrectedSensitivity() const;

	UFUNCTION(BlueprintCallable)
	void SetOutlineEnabled(bool bEnabled);
	bool IsOutlineEnabled() const { return bOutline; }

	UFUNCTION(BlueprintCallable)
	void SetHighlightEnabled(bool bEnabled);
	bool IsHighlightEnabled() const { return bHighlight; }

	void RegisterPPManager(FOnPPSettingChanged&& Callback);


	UPROPERTY(Config, BlueprintReadWrite)
	float RawSensitivity = .5;

	UPROPERTY(Config, BlueprintReadWrite)
	uint8 bAutoFire : 1;

	UPROPERTY(Config, BlueprintReadWrite)
	uint8 bVibration : 1;

protected:
	void PostInitProperties() override;

private:
	FOnPPSettingChanged OnPPSettingChanged;
	
	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString PlayerName;

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float MaxFPS = 60.f;

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bOutline : 1;

	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bHighlight : 1;
};
