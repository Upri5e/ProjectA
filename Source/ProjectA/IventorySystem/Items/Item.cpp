// Cor3 Studios 2020


#include "Item.h"

UItem::UItem()
{
	Weight = 1.f;
	ItemDisplayName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");

}

void UItem::Use(class AMainCharacter* Character)
{

}
