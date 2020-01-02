// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#pragma once

#include "Widget/Feed.h"
#include "KillFeed.generated.h"

struct FKillFeedContent : FFeedContent
{
	FKillFeedContent() = default;
	
	FKillFeedContent(class ASaucewichPlayerState* InVictim,
		ASaucewichPlayerState* InAttacker, AActor* InInflictor)
		: Victim(InVictim), Attacker(InAttacker), Inflictor(InInflictor) {}

	ASaucewichPlayerState* Victim;
	ASaucewichPlayerState* Attacker;
	AActor* Inflictor;
};

UCLASS()
class SAUCEWICH_API UKillFeed : public UFeed
{
	GENERATED_BODY()

public:
	void GetContent(FFeedContent& OutContent) const override;
	void SetContent(const FFeedContent& InContent) override;

private:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	class ASaucewichPlayerState* Victim;
	
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	ASaucewichPlayerState* Attacker;
	
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Content, meta = (AllowPrivateAccess = true))
	AActor* Inflictor;
};
