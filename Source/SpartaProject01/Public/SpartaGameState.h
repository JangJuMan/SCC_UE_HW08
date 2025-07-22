// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAPROJECT01_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Coin")
	int32 CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Level")
	int32 MaxLevels;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	int32 MaxWavesPerLevel = 3;
	UPROPERTY(VisibleInstanceOnly, Category = "Wave")
	int32 CurrentWave = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	float WaveDuration = 20.0f;
	FTimerHandle WaveTimerHandle;
	FTimerHandle RandomExplosionTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	TSubclassOf<class AExplosionEvent> ExplosionEventClass;

	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	UMaterialInstanceDynamic* RoundProgressMaterial;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();
	void UpdateHUD();

	void StartWave();
	void OnWaveTimeUp();
	void ShowWaveEventMessage(const FString& Message);
	void ActivateSpikes();
	void ActivateRandomExplosion();
	void SpawnExplosionNearPlayer();
};
