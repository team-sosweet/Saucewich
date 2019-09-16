// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "JsonData.h"

#include "JsonObject.h"
#include "JsonReader.h"
#include "JsonSerializer.h"

void UJsonData::Create(const FString& Json)
{
	const auto Reader = TJsonReaderFactory<>::Create(Json);
	FJsonSerializer::Deserialize(Reader, JsonObject);
}

void UJsonData::Create(const TSharedPtr<FJsonValue>& JsonValuePtr)
{
	JsonValuePtr->AsArgumentType(JsonObject);
}

void UJsonData::Create(const TSharedPtr<FJsonObject>& JsonObjectPtr)
{
	JsonObject = JsonObjectPtr;
}

bool UJsonData::GetNumberValue(const FString& Key, float& Out)
{
	double Number;
	if (!JsonObject->TryGetNumberField(Key, Number))
		return false;

	Out = Number;
	return true;
}

bool UJsonData::GetIntegerValue(const FString& Key, int32& Out)
{
	return JsonObject->TryGetNumberField(Key, Out);
}

bool UJsonData::GetBooleanValue(const FString& Key, bool& Out)
{
	return JsonObject->TryGetBoolField(Key, Out);
}

bool UJsonData::GetStringValue(const FString& Key, FString& Out)
{
	return JsonObject->TryGetStringField(Key, Out);
}

bool UJsonData::GetObjectValue(const FString& Key, UJsonData*& Out)
{
	const TSharedPtr<FJsonObject>* Object;
	if (!JsonObject->TryGetObjectField(Key, Object))
		return false;

	Out = NewObject<UJsonData>();
	Out->Create(*Object);
	return true;
}

bool UJsonData::GetArrayValue(const FString& Key, TArray<UJsonData*>& Out)
{
	const TArray<TSharedPtr<FJsonValue>>* Arr;
	if (!JsonObject->TryGetArrayField(Key, Arr))
		return false;

	const auto Num = Arr->Num();
	Out.Init(nullptr, Num);

	for (auto Index = 0; Index < Num; Index++)
	{
		Out[Index]->Create((*Arr)[Index]);
	}

	return true;
}
