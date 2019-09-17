// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonData.generated.h"

UCLASS(Blueprintable)
class SAUCEWICH_API UJsonData final : public UObject
{
	GENERATED_BODY()

public:
	void Create(const TSharedPtr<class FJsonValue>& InJsonValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsFloat(float& Out) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsInteger(int32& Out) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsBoolean(bool& Out) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsString(FString& Out) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsArray(TArray<UJsonData*>& Out) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AsObject(TMap<FString, UJsonData*>& Out) const;

	TSharedPtr<class FJsonValue>& operator*()
	{
		return JsonValue;
	}
	
private:
	TSharedPtr<class FJsonValue> JsonValue;
};
