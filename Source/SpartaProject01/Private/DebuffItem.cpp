#include "DebuffItem.h"
#include <SpartaCharacter.h>

void ADebuffItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* Player = Cast<ASpartaCharacter>(Activator))
		{
			Player->ApplyDebuff(DebuffType, DebuffDuration);
			DestroyItem();
		}
	}
}