#include "Character.h"
#include "Event.h"
#include "OverlordMastery.h"
#include "Runtime.h"

Void RTCharacterAddOverlordExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	while (Exp > 0) {
		RTDataOverlordMasteryExpRef Mastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Info.Overlord.Level);
		assert(Mastery);

		RTDataOverlordMasteryExpRef NextMastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Info.Overlord.Level + 1);
		if (!NextMastery) break;

		UInt64 RequiredExp = NextMastery->AccumulatedExp - Character->Info.Overlord.Exp;
		Character->Info.Overlord.Exp += MIN(Exp, RequiredExp);
		Exp -= MIN(Exp, RequiredExp);

		if (Character->Info.Overlord.Exp >= Mastery->AccumulatedExp) {
			Character->Info.Overlord.Point += Mastery->MasteryPointCount;
			Character->Info.Overlord.Level = NextMastery->Level;
			Character->SyncMask.Info = true;
			Character->SyncPriority.Low = true;

			RTCharacterInitializeAttributes(Runtime, Character);

			RTRuntimeBroadcastEvent(
				Runtime,
				RUNTIME_EVENT_CHARACTER_OVERLORD_LEVEL_UP,
				RTRuntimeGetWorldByCharacter(Runtime, Character),
				kEntityIDNull,
				Character->ID,
				Character->Movement.PositionCurrent.X,
				Character->Movement.PositionCurrent.Y
			);
		}
		else {
			break;
		}
	}
}

RTOverlordMasterySlotRef RTCharacterGetOverlordMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex
) {
	for (Int32 Index = 0; Index < Character->OverlordMasteryInfo.Count; Index += 1) {
		if (Character->OverlordMasteryInfo.Slots[Index].MasteryIndex == MasteryIndex) {
			return &Character->OverlordMasteryInfo.Slots[Index];
		}
	}

	return NULL;
}
