#include "BaseObstacle.h"
#include "SpikeTrap.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include <SpartaCharacter.h>

ABaseObstacle::ABaseObstacle()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(SceneRoot);

	ObstacleMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	ObstacleMesh->SetGenerateOverlapEvents(false);
	ObstacleMesh->SetNotifyRigidBodyCollision(true); // "Simulation Generates Hit Events" 코드상

	ObstacleMesh->OnComponentHit.AddDynamic(this, &ABaseObstacle::OnHit);
}

void ABaseObstacle::ActivateObstacle()
{
}

void ABaseObstacle::DeactivateObstacle()
{
}

void ABaseObstacle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 플레이어만 판정
	if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(
			Player,
			DamageAmount,
			nullptr,
			this,
			UDamageType::StaticClass()
		);
	}
}
