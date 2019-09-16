// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonData.generated.h"

UCLASS()
class SAUCEWICH_API UJsonData final : public UObject
{
	GENERATED_BODY()

public:
	void Create(const FString& Json);
	void Create(const TSharedPtr<class FJsonValue>& JsonValuePtr);
	void Create(const TSharedPtr<class FJsonObject>& JsonObjectPtr);
	
	UFUNCTION(BlueprintCallable)
	bool GetNumberValue(const FString& Key, float& Out);

	UFUNCTION(BlueprintCallable)
	bool GetIntegerValue(const FString& Key, int32& Out);

	UFUNCTION(BlueprintCallable)
	bool GetBooleanValue(const FString& Key, bool& Out);

	UFUNCTION(BlueprintCallable)
	bool GetStringValue(const FString& Key, FString& Out);

	UFUNCTION(BlueprintCallable)
	bool GetObjectValue(const FString& Key, UJsonData*& Out);

	UFUNCTION(BlueprintCallable)
	bool GetArrayValue(const FString& Key, TArray<UJsonData*>& Out);

private:
	TSharedPtr<class FJsonObject> JsonObject;
};
