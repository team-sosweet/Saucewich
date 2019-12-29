// Copyright 2019 Othereum. All Rights Reserved.

#include "SaucewichInstance.h"
#include "Engine/World.h"
#include "Entity/ActorPool.h"
#include "Entity/SauceMarker.h"
#include "UserSettings.h"
#include "Engine/Engine.h"

template <class T>
static T* GetOrSpawn(T*& Ptr, UClass* const Class, UWorld* const World)
{
	if (!IsValid(Ptr)) Ptr = World->SpawnActor<T>(Class);
	return Ptr;
}

template <class T>
static T* GetOrSpawn(T*& Ptr, UWorld* const World)
{
	return GetOrSpawn(Ptr, T::StaticClass(), World);
}

USaucewichInstance* USaucewichInstance::Get(const UObject* const WorldContextObj)
{
	return WorldContextObj->GetWorld()->GetGameInstanceChecked<USaucewichInstance>();
}

AActorPool* USaucewichInstance::GetActorPool() const
{
	return GetOrSpawn(ActorPool, GetWorld());
}

ASauceMarker* USaucewichInstance::GetSauceMarker() const
{
	return GetOrSpawn(SauceMarker, SauceMarkerClass.LoadSynchronous(), GetWorld());
}

bool USaucewichInstance::PopNetworkError(TEnumAsByte<ENetworkFailure::Type>& Type, FString& Msg)
{
	if (!LastNetworkError.bOccured) return false;
	
	Msg = MoveTemp(LastNetworkError.Msg);
	Type = LastNetworkError.Type;
	
	LastNetworkError.bOccured = false;
	return true;
}

void USaucewichInstance::Init()
{
	Super::Init();

	UserSettings = NewObject<UUserSettings>();
	UserSettings->CommitMaxFPS();

	GEngine->NetworkFailureEvent.AddUObject(this, &USaucewichInstance::OnNetworkError);
}

void USaucewichInstance::OnNetworkError(UWorld*, UNetDriver*, const ENetworkFailure::Type Type, const FString& Msg)
{
	LastNetworkError.Msg = Msg;
	LastNetworkError.Type = Type;
	LastNetworkError.bOccured = true;
}
