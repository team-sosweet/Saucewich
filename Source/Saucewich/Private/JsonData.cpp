// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "JsonData.h"

#include "JsonObject.h"
#include "JsonReader.h"
#include "JsonSerializer.h"

void UJsonData::Create(const TSharedPtr<FJsonValue>& InJsonValue)
{
	JsonValue = InJsonValue;
}

bool UJsonData::AsFloat(float& Out) const
{
	double Value;
	if (!JsonValue->TryGetNumber(Value))
		return false;

	Out = Value;
	return true;
}

bool UJsonData::AsInteger(int32& Out) const
{
	return JsonValue->TryGetNumber(Out);
}

bool UJsonData::AsBoolean(bool& Out) const
{
	return JsonValue->TryGetBool(Out);
}

bool UJsonData::AsString(FString& Out) const
{
	return JsonValue->TryGetString(Out);
}

bool UJsonData::AsArray(TArray<UJsonData*>& Out) const
{
	const TArray<TSharedPtr<FJsonValue>>* Arr;
	if (!JsonValue->TryGetArray(Arr))
		return false;

	const auto Num = Arr->Num();
	Out.Init(nullptr, Num);

	for (auto Index = 0; Index < Num; Index++)
	{
		Out[Index]->Create((*Arr)[Index]);
	}

	return true;
}

bool UJsonData::AsObject(TMap<FString, UJsonData*>& Out) const
{
	const TSharedPtr<FJsonObject>* Object;
	if (!JsonValue->TryGetObject(Object))
		return false;

	Out.Empty();
	const auto Values = (*Object)->Values;
	
	for (const auto& Value : Values)
	{
		auto Data = NewObject<UJsonData>();
		Data->Create(Value.Value);
		Out.Add(Value.Key, Data);
	}
	
	return true;
}