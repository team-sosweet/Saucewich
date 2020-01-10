// Copyright 2019-2020 Seokjin Lee. All Rights Reserved.

#include "Entity/RepDat.h"
#include "Net/UnrealNetwork.h"

ARepDat::ARepDat()
{
	bReplicates = true;
}

void ARepDat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
}
