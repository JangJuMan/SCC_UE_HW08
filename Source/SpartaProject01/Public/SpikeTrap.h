#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "SpikeTrap.generated.h"

UCLASS()
class SPARTAPROJECT01_API ASpikeTrap : public ABaseObstacle
{
	GENERATED_BODY()

public:
	ASpikeTrap();

	void ActivateObstacle() override;
	void DeactivateObstacle() override;

protected:
	void BeginPlay() override;


	void StartCycle();
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void PlayAnimation();
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void StopAnimaion();

	FTimerHandle SpikeTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Spike Trap")
	float ActivateDuration = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Spike Trap")
	float DeactivateDuration = 2.0f;


};
