#include "ExplosionEvent.h"
#include <Kismet/GameplayStatics.h>

AExplosionEvent::AExplosionEvent()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AExplosionEvent::BeginPlay()
{
	Super::BeginPlay();
	Explode();
	
}

void AExplosionEvent::Explode()
{
	if (ExplosionParticle)
	{	
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), GetActorRotation(), true);
	}
	if(ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActors; // 무시할 액터 없음
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), 
		ExplosionDamage, 
		GetActorLocation(), 
		ExplosionRadius, 
		UDamageType::StaticClass(),
		IgnoredActors,
		this, 
		nullptr,
		true);

	Destroy(); // 폭발 후 액터 제거
}

