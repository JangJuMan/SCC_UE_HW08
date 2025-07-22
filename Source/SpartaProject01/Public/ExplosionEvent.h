#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionEvent.generated.h"

UCLASS()
class SPARTAPROJECT01_API AExplosionEvent : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	USoundBase* ExplosionSound;


public:	
	AExplosionEvent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionRadius = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage = 20.0f;

	void Explode();
	

};
