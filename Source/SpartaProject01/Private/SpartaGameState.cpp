#include "SpartaGameState.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include <SpartaPlayerController.h>
#include <Components/TextBlock.h>
#include "Blueprint/UserWidget.h"
#include <Components/Image.h>
#include <SpartaCharacter.h>
#include "SpikeTrap.h"
#include "ExplosionEvent.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	//LevelDuration = 20.0f;
	WaveDuration = 20.0f;
	CurrentLevelIndex = 0;
	CurrentWave = 0;
	MaxLevels = 3;
	MaxWavesPerLevel = 3;
	RoundProgressMaterial = nullptr;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains(TEXT("MenuLevel")))
	{
		return;	// 메뉴레벨에서는 타이머 비활성화
	}

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.05f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}

	CurrentWave = 0;
	StartWave();

}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected : %d / %d"), CollectedCoinCount, SpawnedCoinCount);

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		OnWaveTimeUp();
		//EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	//GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UImage* TimerProgress = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("TimerProgress"))))
				{
					if (!RoundProgressMaterial)
					{
						RoundProgressMaterial = TimerProgress->GetDynamicMaterial();	// 한 번만 열기
					}
					if (RoundProgressMaterial)
					{
						float RemainTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
						float ProgressPercent = RemainTime / WaveDuration;
						RoundProgressMaterial->SetScalarParameterValue("Percent", ProgressPercent);
					}
	
				}

				if (UTextBlock* ScoreText= Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex+1)));
				}

				if (UTextBlock* WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave)));
				}
			}

		}
		if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(PlayerController->GetPawn()))
		{
			Player->UpdateDebuffUI();
		}
	}
}

void ASpartaGameState::StartWave()
{
	CurrentWave++;

	if (CurrentWave == 2)
	{
		ActivateSpikes();
		ShowWaveEventMessage(TEXT("스파이크 함정이 활성화되었습니다!"));
	}
	else if (CurrentWave == 3)
	{
		ActivateRandomExplosion();
		ShowWaveEventMessage(TEXT("무작위 폭발이 시작됩니다"));
	}
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	int32 BaseSpawnCount = 30;
	int32 ItemToSpawn = BaseSpawnCount + (CurrentWave * 10);
	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);
}

void ASpartaGameState::OnWaveTimeUp()
{
	if (CurrentWave < MaxWavesPerLevel)
	{
		StartWave();
	}
	else
	{
		EndLevel();
	}
}

void ASpartaGameState::ShowWaveEventMessage(const FString& Message)
{


	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(!PlayerController) return;
	
	ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
	if(!SpartaPlayerController) return;

	UUserWidget* HUD = SpartaPlayerController->GetHUDWidget();
	if(!HUD) return;

	UTextBlock* WaveAlert = Cast<UTextBlock>(HUD->GetWidgetFromName(TEXT("WaveAlertText")));
	if(!WaveAlert) return;

	WaveAlert->SetText(FText::FromString(Message));
	WaveAlert->SetVisibility(ESlateVisibility::Visible);

	FTimerHandle HideAlertTimerHandle;
	TWeakObjectPtr<UTextBlock> WeakWaveAlert(WaveAlert);
	GetWorld()->GetTimerManager().SetTimer(HideAlertTimerHandle, [WeakWaveAlert]()
		{
			if (WeakWaveAlert.IsValid())
			{
				WeakWaveAlert->SetVisibility(ESlateVisibility::Collapsed);
			}
		}, 3.0f, false);

}

void ASpartaGameState::ActivateSpikes()
{
	TArray<AActor*> FoundSpikes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpikeTrap::StaticClass(), FoundSpikes);

	for (AActor* Spike : FoundSpikes)
	{
		if (ASpikeTrap* Trap = Cast<ASpikeTrap>(Spike))
		{
			Trap->ActivateObstacle();
		}
	}
}

void ASpartaGameState::ActivateRandomExplosion()
{
	float Duration = 3.5f - CurrentLevelIndex;
	GetWorldTimerManager().SetTimer(
		RandomExplosionTimerHandle, 
		this, 
		&ASpartaGameState::SpawnExplosionNearPlayer,
		Duration,	// FORDEBUG
		true);
}

void ASpartaGameState::SpawnExplosionNearPlayer()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	
	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;	

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	float ExplosionRadius = 800.0f; 
	FVector RandomOffset = FVector(
		FMath::FRandRange(-ExplosionRadius, ExplosionRadius),
		FMath::FRandRange(-ExplosionRadius, ExplosionRadius),
		0.0f
	);
	FVector SpawnLocation = PlayerLocation + RandomOffset;

	if (ExplosionEventClass)
	{
		GetWorld()->SpawnActor<AActor>(ExplosionEventClass, SpawnLocation, FRotator::ZeroRotator);
	}
}
