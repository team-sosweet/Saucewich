// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameMode.h"

#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "Saucewich.h"
#include "Online/SaucewichGameState.h"
#include "Player/SaucewichPlayerState.h"

ASaucewichGameMode::ASaucewichGameMode()
{
	// AGameMode::Tick()에서 매 틱마다 매치 상태 업데이트를 하지만 그럴 필요가 없으므로
	PrimaryActorTick.bCanEverTick = false;
}

void ASaucewichGameMode::UpdateMatchState()
{
	if (GetMatchState() == MatchState::WaitingToStart)
		if (ReadyToStartMatch())
			StartMatch();
	if (GetMatchState() == MatchState::InProgress)
		if (ReadyToEndMatch())
			EndMatch();
}

void ASaucewichGameMode::SetPlayerRespawnTimer(ASaucewichPlayerController* const PC) const
{
	PC->SetRespawnTimer(MinRespawnDelay);
}

void ASaucewichGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	State = GetGameState<ASaucewichGameState>();
	GUARANTEE(State != nullptr);
}

APlayerController* ASaucewichGameMode::SpawnPlayerController(const ENetRole InRemoteRole, const FString& Options)
{
	const auto PC = Super::SpawnPlayerController(InRemoteRole, Options);
	if (!PC) return nullptr;
	
	if (State)
	{
		LastTeam = UGameplayStatics::GetIntOption(Options, "Team", 0);
		if (LastTeam == 0) LastTeam = State->GetMinPlayerTeam();
		if (const auto PS = PC->GetPlayerState<ASaucewichPlayerState>())
		{
			PS->SetTeam(LastTeam);
		}
	}
	
	return PC;
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

bool ASaucewichGameMode::FindInactivePlayer(APlayerController* const PC)
{
	const auto bFound = Super::FindInactivePlayer(PC);
	
	if (bFound)
	{
		if (const auto PS = PC->GetPlayerState<ASaucewichPlayerState>())
		{
			// PlayerState가 초기화되었으므로 팀을 재설정
			PS->SetTeam(LastTeam);
		}
	}
	
	return bFound;
}

void ASaucewichGameMode::RestartPlayerAtPlayerStart(AController* const NewPlayer, AActor* const StartSpot)
{
	if (!IsValid(NewPlayer)) return;
	if (!StartSpot) return;
	if (MustSpectate(Cast<APlayerController>(NewPlayer))) return;

	if (!NewPlayer->GetPawn() && GetDefaultPawnClassForController(NewPlayer))
	{
		NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, StartSpot));
	}

	if (!NewPlayer->GetPawn())
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		InitStartSpot(StartSpot, NewPlayer);
		auto& Transform = StartSpot->GetRootComponent()->GetComponentTransform();
		NewPlayer->GetPawn()->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());
		FinishRestartPlayer(NewPlayer, StartSpot->GetActorRotation());
	}
}

void ASaucewichGameMode::SetPlayerDefaults(APawn* const PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	if (const auto PS = PlayerPawn->GetPlayerState<ASaucewichPlayerState>())
	{
		PS->GiveWeapons();
	}
}
