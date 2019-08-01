// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "SaucewichGameState.h"
#include "SaucewichPlayerState.h"
#include "TpsCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogSaucewichGameMode, Log, All)

void ASaucewichGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	State = GetGameState<ASaucewichGameState>();
	if (!State)
	{
		UE_LOG(LogSaucewichGameMode, Error, TEXT("GameState가 유효하지 않습니다. GameMode가 비활성화 됩니다. Class가 SaucewichGameState가 맞는지 확인해주세요."));
	}
}

void ASaucewichGameMode::SetPlayerDefaults(APawn* const PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	if (const auto Character = Cast<ATpsCharacter>(PlayerPawn))
	{
		GiveWeapons(Character);
	}
}

AActor* ASaucewichGameMode::ChoosePlayerStart_Implementation(AController* const Player)
{
	const auto PawnClass = GetDefaultPawnClassForController(Player);
	const auto PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	const auto World = GetWorld();

	TCHAR Tag[] = TEXT("0");
	Tag[0] = TEXT('0') + Player->GetPlayerState<ASaucewichPlayerState>()->GetTeam();

	// 4개의 TArray는 우선순위를 나타내며 0번이 가장 높다.
	TArray<APlayerStart*> StartPoints[4];

	for (TActorIterator<APlayerStart> It{World}; It; ++It)
	{
		const auto PlayerStart = *It;

#if WITH_EDITOR
		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			return PlayerStart;
		}
#endif

		const auto bMyTeam = PlayerStart->PlayerStartTag == Tag;
		auto ActorLocation = PlayerStart->GetActorLocation();
		const auto ActorRotation = PlayerStart->GetActorRotation();
		if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
		{
			(bMyTeam ? StartPoints[0] : StartPoints[2]).Add(PlayerStart);
		}
		else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
		{
			(bMyTeam ? StartPoints[1] : StartPoints[3]).Add(PlayerStart);
		}
	}

	for (const auto& Starts : StartPoints)
	{
		if (Starts.Num() > 0)
		{
			return Starts[FMath::RandHelper(Starts.Num())];
		}
	}

	return nullptr;
}

APlayerController* ASaucewichGameMode::SpawnPlayerController(const ENetRole InRemoteRole, const FString& Options)
{
	const auto PC = Super::SpawnPlayerController(InRemoteRole, Options);
	if (State && PC)
	{
		auto Team = UGameplayStatics::GetIntOption(Options, "Team", 0);
		if (Team == 0) Team = State->GetMinPlayerTeam();
		PC->GetPlayerState<ASaucewichPlayerState>()->SetTeam(Team);
	}
	return PC;
}
