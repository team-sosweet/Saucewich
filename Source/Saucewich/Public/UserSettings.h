// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UserSettings.generated.h"

enum class ENameValidity : uint8;

UCLASS(Config=UserSettings)
class SAUCEWICH_API UUserSettings : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	ENameValidity SetPlayerName(const FString& NewPlayerName);
	const FString& GetPlayerName() const { return PlayerName; }
	
protected:
	void PostInitProperties() override;

private:
	UPROPERTY(Config, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString PlayerName;
};
