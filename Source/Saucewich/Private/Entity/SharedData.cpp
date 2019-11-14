// Copyright 2019 Team Sosweet. All Rights Reserved.


#include "SharedData.h"

// Sets default values
ASharedData::ASharedData()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASharedData::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASharedData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

