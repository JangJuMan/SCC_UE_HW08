#include "SlowingItem.h"

ASlowingItem::ASlowingItem()
{
	DebuffType = EDebuffType::Slow;
	DebuffDuration = 5.0f; // Duration of the debuff in seconds
	ItemType = "Slowing";
}
