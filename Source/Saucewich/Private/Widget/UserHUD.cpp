// Copyright 2019 Othereum. All Rights Reserved.

#include "Widget/UserHUD.h"

#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"

#include "Player/SaucewichPlayerState.h"
#include "Player/TpsCharacter.h"
#include "Player/SaucewichPlayerController.h"

void UUserHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	VisibilityDelegate.BindDynamic(this, &UUserHUD::GetHUDVisibility);
	ShowAngleRadian = FMath::DegreesToRadians(ShowAngle * 0.5f);
}

void UUserHUD::Init(ATpsCharacter* const InOwnerPawn)
{
	OwnerPawn = InOwnerPawn;

	OwnerPawn->OnCharacterSpawn.AddDynamic(this, &UUserHUD::OnSpawn);
	OwnerPawn->OnCharacterDeath.AddDynamic(this, &UUserHUD::OnDeath);

	FOnPlayerStateSpawnedSingle WithPlayerState;
	WithPlayerState.BindDynamic(this, &UUserHUD::InitPlayerState);

	const auto PC = CastChecked<ASaucewichPlayerController>(GetOwningPlayer());
	PC->SafePlayerState(WithPlayerState);
}

void UUserHUD::InitPlayerState(ASaucewichPlayerState* const PlayerState)
{
	OnInit(PlayerState);
	PlayerState->OnTeamChangedDelegate.AddDynamic(this, &UUserHUD::OnOwnerTeamChanged);
	OnOwnerTeamChanged(PlayerState->GetTeam());
}

ESlateVisibility UUserHUD::GetHUDVisibility()
{
	const auto GS = CastChecked<AGameState>(GetWorld()->GetGameState(), ECastCheckedType::NullAllowed);
	if (!GS || !GS->IsMatchInProgress() || !OwnerPawn || !LocalPawn || OwnerPawn == LocalPawn || IsDead)
	{
		return ESlateVisibility::Hidden;
	}
	if (OwnerTeam == LocalTeam)
	{
		return ESlateVisibility::SelfHitTestInvisible;
	}

	const auto OwnerLocation = OwnerPawn->GetActorLocation();
	const auto LocalLocation = LocalPawn->GetActorLocation();
	const auto DistanceVec = FVector2D(OwnerLocation - LocalLocation);
	const auto DistanceSquared = DistanceVec.SizeSquared();

	if (DistanceSquared > FMath::Square(ShowDistance))
	{
		return ESlateVisibility::Hidden;
	}

	const auto LocalDirection = LocalPawn->GetBaseAimRotation().Vector();

	const auto AimAngle = FMath::Atan2(LocalDirection.Y, LocalDirection.X);
	const auto PosAngle = FMath::Atan2(DistanceVec.Y, DistanceVec.X);
	const auto Angle = FMath::Abs(AimAngle - PosAngle);

	if (Angle > ShowAngleRadian)
	{
		return ESlateVisibility::Hidden;
	}

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActors(TArray<AActor*>{OwnerPawn, LocalPawn});
	if (GetWorld()->LineTraceSingleByChannel(HitResult, LocalLocation, OwnerLocation, ECC_Visibility))
	{
		return ESlateVisibility::Hidden;
	}

	return ESlateVisibility::SelfHitTestInvisible;
}

void UUserHUD::OnOwnerTeamChanged(const uint8 NewTeam)
{
	OwnerTeam = NewTeam;
}

void UUserHUD::OnLocalTeamChanged(const uint8 NewTeam)
{
	LocalTeam = NewTeam;
}

void UUserHUD::OnSpawn()
{
	IsDead = false;
}

void UUserHUD::OnDeath()
{
	IsDead = true;
}
