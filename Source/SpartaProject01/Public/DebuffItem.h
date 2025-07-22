#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DebuffItem.generated.h"

UENUM(BlueprintType)
enum class EDebuffType : uint8
{
	Slow,
	Reverse,
	Blind
};

UCLASS()
class SPARTAPROJECT01_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
protected:
	EDebuffType DebuffType;
	float DebuffDuration;

public:
	virtual void ActivateItem(AActor* Activator) override;
};
