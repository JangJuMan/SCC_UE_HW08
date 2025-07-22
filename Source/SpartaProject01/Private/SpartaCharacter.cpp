// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SpartaPlayerController.h"
#include "Components/WidgetComponent.h"
#include <Components/TextBlock.h>
#include "SpartaGameState.h"
#include <Components/ProgressBar.h>
#include "GameFramework/PlayerController.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 350.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace((EWidgetSpace::Screen));

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * MoveDebuffRatio;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	Tags.Add(FName("Player"));
}

int32 ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move 
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);	// ActualDamage를 깎아야 하는 것 아닌지?
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}

}


void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	FVector2D MoveInput = value.Get<FVector2D>();
	if (bIsReverse) MoveInput *= -1.f;	// Reverse 디버프가 적용된 경우 입력 반전

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	// TODO: Reverse 디버프가 적용된 경우 입력 반전
	//if (bIsReverse) LookInput *= -1.f;	// Reverse 디버프가 적용된 경우 입력 반전
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * MoveDebuffRatio;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * MoveDebuffRatio;
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget)	return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance)	return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}

	if (UProgressBar* HPBar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("ProgressHP"))))
	{
		HPBar->SetPercent(Health/MaxHealth);
	}
}

void ASpartaCharacter::ApplyDebuff(EDebuffType Type, float Duration)
{
	switch (Type)
	{
	case EDebuffType::Slow:
		ApplySlowDebuff(Duration);
		break;
	case EDebuffType::Reverse:
		ApplyReverseDebuff(Duration);
		break;
	case EDebuffType::Blind:
		ApplyBlindDebuff(Duration);
		break;
	}
	UpdateDebuffUI();
}

void ASpartaCharacter::ApplySlowDebuff(float Duration)
{
	bIsSlow = true;
	MoveDebuffRatio = 0.5f;
	GetWorldTimerManager().ClearTimer(SlowDebuffTimer);	// 중첩 연장 지원
	GetWorldTimerManager().SetTimer(
		SlowDebuffTimer, 
		this, 
		&ASpartaCharacter::ClearSlowDebuff,
		Duration,
		false);
}

void ASpartaCharacter::ApplyReverseDebuff(float Duration)
{
	bIsReverse = true;
	GetWorldTimerManager().ClearTimer(ReverseDebuffTimer);	// 중첩 연장 지원
	GetWorldTimerManager().SetTimer(
		ReverseDebuffTimer, 
		this, 
		&ASpartaCharacter::ClearReverseDebuff,
		Duration,
		false);
}

void ASpartaCharacter::ApplyBlindDebuff(float Duration)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if(!PC) return;

	ASpartaPlayerController* SPC = Cast<ASpartaPlayerController>(PC);
	if(!SPC) return;

	UUserWidget* HUD = SPC->GetHUDWidget();
	if(!HUD) return;


	if (!bIsBlind)	// 이미 blind인 경우 지속시간만 증가
	{
		UFunction* PlayFadeInAnim = HUD->FindFunction(FName("PlayBlindOverlayFadeIn"));
		if (PlayFadeInAnim)
		{
			HUD->ProcessEvent(PlayFadeInAnim, nullptr);
		}
	}



	bIsBlind = true;
	GetWorldTimerManager().ClearTimer(BlindDebuffTimer);	// 중첩 연장 지원
	GetWorldTimerManager().SetTimer(
		BlindDebuffTimer, 
		this, 
		&ASpartaCharacter::ClearBlindDebuff,
		Duration,
		false);
}

void ASpartaCharacter::UpdateDebuffUI()
{	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	ASpartaPlayerController* SPC = Cast<ASpartaPlayerController>(PC);
	if(!SPC) return;

	UUserWidget* HUD = SPC->GetHUDWidget();
	if(!HUD) return;

	UTextBlock* DebuffText = Cast<UTextBlock>(HUD->GetWidgetFromName(TEXT("DebuffTime")));
	if(!DebuffText) return;

	if (!bIsSlow && !bIsReverse && !bIsBlind)
	{
		DebuffText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FString AllDebuffs = "[Debuff]\n";

	float SlowRemain = GetWorldTimerManager().GetTimerRemaining(SlowDebuffTimer);
	float ReverseRemain = GetWorldTimerManager().GetTimerRemaining(ReverseDebuffTimer);
	float BlindRemain = GetWorldTimerManager().GetTimerRemaining(BlindDebuffTimer);

	if (bIsSlow && SlowRemain > 0.0f)
	{
		AllDebuffs += FString::Printf(TEXT("Slow: %.1f\n"), SlowRemain);
	}
	if (bIsReverse && ReverseRemain > 0.0f)
	{
		AllDebuffs += FString::Printf(TEXT("Reverse: %.1f\n"), ReverseRemain);
	}
	if (bIsBlind && BlindRemain > 0.0f)
	{
		AllDebuffs += FString::Printf(TEXT("Blind: %.1f\n"), BlindRemain);
	}
	
	DebuffText->SetText(FText::FromString(AllDebuffs));
	DebuffText->SetVisibility(ESlateVisibility::Visible);
}

void ASpartaCharacter::ClearSlowDebuff()
{
	bIsSlow = false;
	MoveDebuffRatio = 1.0f;
	UpdateDebuffUI();
}

void ASpartaCharacter::ClearReverseDebuff()
{
	bIsReverse = false;
	UpdateDebuffUI();
}

void ASpartaCharacter::ClearBlindDebuff()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	ASpartaPlayerController* SPC = Cast<ASpartaPlayerController>(PC);
	if (!SPC) return;

	UUserWidget* HUD = SPC->GetHUDWidget();
	if (!HUD) return;

	UFunction* PlayFadeOutAnim = HUD->FindFunction(FName("PlayBlindOverlayFadeOut"));
	if (PlayFadeOutAnim)
	{
		HUD->ProcessEvent(PlayFadeOutAnim, nullptr);
	}

	bIsBlind = false;
	UpdateDebuffUI();
}

