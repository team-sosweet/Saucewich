// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Widget/KillFeed.h"

void UKillFeed::GetContent(FFeedContent& OutContent) const
{
	auto& Content = static_cast<FKillFeedContent&>(OutContent);

	Content.Victim = Victim;
	Content.Attacker = Attacker;
	Content.Inflictor = Inflictor;
}

void UKillFeed::SetContent(const FFeedContent& InContent)
{
	const auto& Content = static_cast<const FKillFeedContent&>(InContent);

	Victim = Content.Victim;
	Attacker = Content.Attacker;
	Inflictor = Content.Inflictor;

	Super::SetContent(InContent);
}
