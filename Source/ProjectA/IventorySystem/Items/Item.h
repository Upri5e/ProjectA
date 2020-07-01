// Cor3 Studios 2020

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

class Item;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class PROJECTA_API UItem : public UObject
{
	GENERATED_BODY()
	
public:
	
	UItem();

	virtual class UWorld* GetWorld() const { return World; };

	UPROPERTY(Transient)
	class UWorld* World;
	// The text for using the item (Equip, Eat, etc)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	FText UseActionText;

	//The Mesh to display for this item pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class UStaticMesh* PickupMesh;

	//Thumbnail for this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class UTexture2D* Thumbnail;

	//The display name for this item in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	FText ItemDisplayName;
	
	//An optional description for the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items", meta = (MultiLine = true))
	FText ItemDescription;

	//The Weight of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items", meta = (ClampMin = 0.0))
	float Weight;

	// The inventory that owns the this item
	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	virtual void Use(class AMainCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AMainCharacter* Character);
};
