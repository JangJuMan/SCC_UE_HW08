#include "SpikeTrap.h"
#include <Kismet/GameplayStatics.h>

ASpikeTrap::ASpikeTrap()
{
	
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	DeactivateObstacle();
}

void ASpikeTrap::ActivateObstacle()
{
	ObstacleMesh->SetVisibility(true);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayAnimation();
}

void ASpikeTrap::DeactivateObstacle()
{
	ObstacleMesh->SetVisibility(false);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASpikeTrap::StartCycle()
{
	DeactivateObstacle();
}
