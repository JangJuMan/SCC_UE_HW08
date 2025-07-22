#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <DebuffItem.h>
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UWidgetComponent;

UCLASS()
class SPARTAPROJECT01_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* OverheadWidget;


	UFUNCTION(BlueprintPure, Category="Health")
	int32 GetHealth() const;
	UFUNCTION(BlueprintCallable, Category="Health")
	void AddHealth(float Amount);

	void ApplyDebuff(EDebuffType Type, float Duration);
	void UpdateDebuffUI();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Health")
	float Health;

	FTimerHandle SlowDebuffTimer;
	FTimerHandle ReverseDebuffTimer;
	FTimerHandle BlindDebuffTimer;


	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath();
	virtual void BeginPlay() override;


	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);  

	void UpdateOverheadHP();
	
	void ApplySlowDebuff(float Duration);
	void ApplyReverseDebuff(float Duration);
	void ApplyBlindDebuff(float Duration);
	void ClearSlowDebuff();
	void ClearReverseDebuff();
	void ClearBlindDebuff();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	bool bIsSlow = false;
	bool bIsReverse = false;
	bool bIsBlind = false;
	float MoveDebuffRatio = 1.0f;
};
