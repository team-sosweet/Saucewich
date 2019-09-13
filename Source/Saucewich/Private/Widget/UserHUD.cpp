// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "Widget/UserHUD.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Player/SaucewichPlayerState.h"

void UUserHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	VisibilityDelegate.BindDynamic(this, &UUserHUD::GetHUDVisibility);
	ShowAngleRadian = FMath::DegreesToRadians(ShowAngle * 0.5f);
}

void UUserHUD::Init(APawn* InOwnerPawn)
{
	OwnerPawn = InOwnerPawn;
	LocalPawn = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();

	BindPlayerState(OwnerPawn, [this](ASaucewichPlayerState* PS)
		{
			OnInit(PS);
			PS->OnTeamChangedDelegate.AddDynamic(this, &UUserHUD::OnOwnerTeamChanged);
			OnOwnerTeamChanged(PS->GetTeam());
		});

	BindPlayerState(LocalPawn, [this](ASaucewichPlayerState* PS)
		{
			PS->OnTeamChangedDelegate.AddDynamic(this, &UUserHUD::OnLocalTeamChanged);
			OnLocalTeamChanged(PS->GetTeam());
		});
}

ESlateVisibility UUserHUD::GetHUDVisibility()
{
	if (OwnerPawn == LocalPawn)
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

void UUserHUD::BindPlayerState(const APawn* InPawn, const TFunction<void(ASaucewichPlayerState*)>& Callback)
{
	if (const auto PS = InPawn->GetPlayerState<ASaucewichPlayerState>())
	{
		Callback(PS);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, InPawn, Callback]
			{
				BindPlayerState(InPawn, Callback);
			});
	}
}
