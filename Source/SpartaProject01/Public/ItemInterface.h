// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPARTAPROJECT01_API IItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,	// Overlap한 자기자신
		AActor* OtherActor,						// Overlap한 상대방 액터
		UPrimitiveComponent* OtherComp,			// 충돌을 일으킨 원인 컴포넌트
		int32 OtherBodyIndex,					// 
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0;
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;

	// 지뢰, 힐링, 코인
	// 힐링 - 즉시 발동형 - 오버랩
	// 지뢰 - 범위 내에 오버랩 - 발동 5초뒤 폭발 - 오버랩 - 데미지
};
