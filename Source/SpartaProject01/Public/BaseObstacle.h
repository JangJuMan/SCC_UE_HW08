#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObstacle.generated.h"

class USphereComponent;

UCLASS()
class SPARTAPROJECT01_API ABaseObstacle : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ObstacleMesh;
	
public:	
	ABaseObstacle();

	virtual void ActivateObstacle();
	virtual void DeactivateObstacle();

protected:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle Settings")
	int32 DamageAmount = 1;

	
};
