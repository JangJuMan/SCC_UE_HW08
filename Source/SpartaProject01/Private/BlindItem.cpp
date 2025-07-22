#include "BlindItem.h"

ABlindItem::ABlindItem()
{
	DebuffType = EDebuffType::Blind;
	DebuffDuration = 5.0f; // Duration of the blind effect in seconds
	ItemType = "Blind";
}
