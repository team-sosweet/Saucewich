// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonData.generated.h"

UCLASS(Blueprintable)
class SAUCEWICH_API UJsonData : public UObject
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeFloatData(float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeIntegerData(int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeBooleanData(bool Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeStringData(const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeArrayData(const TArray<UJsonData*>& Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Create JsonData")
	static UJsonData* MakeObjectData(const TMap<FString, UJsonData*>& Value);

	TSharedPtr<class FJsonValue>& operator*()
	{
		return JsonValue;
	}
	
private:
	static TArray<TSharedPtr<FJsonValue>> GetValues(const TArray<UJsonData*>& Array);
	static TArray<TSharedPtr<FJsonValue>> GetValues(const TMap<FString, UJsonData*>& Map);
	
	TSharedPtr<class FJsonValue> JsonValue;
};
