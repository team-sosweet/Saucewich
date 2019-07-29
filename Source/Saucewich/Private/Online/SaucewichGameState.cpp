// Copyright 2019 Team Sosweet. All Rights Reserved.

#include "SaucewichGameState.h"
#include "SaucewichPlayerState.h"

TArray<ASaucewichPlayerState*> ASaucewichGameState::GetPlayers(const uint8 Team) const
{
	TArray<ASaucewichPlayerState*> Players;
	for (const auto PlayerState : PlayerArray)
		if (const auto Player = Cast<ASaucewichPlayerState>(PlayerState))
			if (Player->GetTeam() == Team)
				Players.Add(Player);
	return Players;
}

TArray<AActor*> ASaucewichGameState::GetCharacters(const uint8 Team) const
{
	TArray<AActor*> Characters;
	for (const auto PlayerState : PlayerArray)
		if (const auto Player = Cast<ASaucewichPlayerState>(PlayerState))
			if (Player->GetTeam() == Team)
				if (const auto Character = Player->GetPawn())
					Characters.Add(Character);
	return Characters;
}
