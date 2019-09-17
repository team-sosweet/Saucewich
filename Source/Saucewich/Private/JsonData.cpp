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

UJsonData* UJsonData::MakeFloatData(float Value)
{
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueNumber>(Value);
	return Data;
}

UJsonData* UJsonData::MakeIntegerData(int32 Value)
{
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueNumber>(Value);
	return Data;
}

UJsonData* UJsonData::MakeBooleanData(bool Value)
{
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueBoolean>(Value);
	return Data;
}

UJsonData* UJsonData::MakeStringData(const FString& Value)
{
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueString>(Value);
	return Data;
}

UJsonData* UJsonData::MakeArrayData(const TArray<UJsonData*>& Value)
{
	const auto Arr = GetValues(Value);
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueArray>(Arr);
	return Data;
}

UJsonData* UJsonData::MakeObjectData(const TMap<FString, UJsonData*>& Value)
{
	const auto Arr = GetValues(Value);

	FString JsonString;
	const auto Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(Arr, Writer);
	Writer->Close();

	const auto Reader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(Reader, JsonObject);
	
	const auto Data = NewObject<UJsonData>();
	**Data = MakeShared<FJsonValueObject>(JsonObject);
	return Data;
}

TArray<TSharedPtr<FJsonValue>> UJsonData::GetValues(const TArray<UJsonData*>& Array)
{
	TArray<TSharedPtr<FJsonValue>> Ret;

	for (const auto& Elem : Array)
	{
		Ret.Add(**Elem);
	}

	return Ret;
}

TArray<TSharedPtr<FJsonValue>> UJsonData::GetValues(const TMap<FString, UJsonData*>& Map)
{
	TArray<TSharedPtr<FJsonValue>> Ret;

	for (const auto& Elem : Map)
	{
		Ret.Add(**Elem.Value);
	}

	return Ret;
}
