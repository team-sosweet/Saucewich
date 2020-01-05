// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "GameMode/DSDefGM.h"
#include "Engine/World.h"
#include "GameMode/SaucewichGameMode.h"
#include "SaucewichInstance.h"

#if WITH_GAMELIFT
	#include "GameLiftServerSDK.h"
#endif

ADSDefGM::ADSDefGM()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADSDefGM::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bStartGame)
	{
		StartGame();
		bStartGame = false;
	}
}

void ADSDefGM::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
#if WITH_GAMELIFT
	UE_LOG(LogGameMode, Warning, TEXT("Someone tried to join while server is not ready!"));
	UE_LOG(LogGameMode, Warning, TEXT("Options: %s"), *Options);
	UE_LOG(LogGameMode, Warning, TEXT("Address: %s"), *Address);
	ErrorMessage = TEXT("Server is not ready");
#else
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (ErrorMessage.IsEmpty()) StartGame();
#endif
}

#if WITH_GAMELIFT
namespace GameLift
{
	extern void Check(const FGameLiftGenericOutcome& Outcome);
}
#endif

void ADSDefGM::StartGame() const
{
	auto&& GameModes = USaucewichInstance::Get(this)->GetGameModes();
	const auto GmClass = GameModes[FMath::RandHelper(GameModes.Num())];
	const auto DefGm = GetDefault<ASaucewichGameMode>(GmClass.LoadSynchronous());

	auto&& Maps = DefGm->GetData().Maps;
	auto&& NewMap = Maps[FMath::RandHelper(Maps.Num())];

	const auto URL = FString::Printf(TEXT("%s?game=%s?listen"), *NewMap.GetAssetName(), *GmClass->GetPathName());
	GetWorld()->ServerTravel(URL, true);
}
