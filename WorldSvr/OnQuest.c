#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(QUEST_BEGIN) {
	S2C_DATA_QUEST_BEGIN* Response = PacketBufferInit(Connection->PacketBuffer, S2C, QUEST_BEGIN);
	Response->Result = 0;

	if (!Character) goto error;

	if (RTCharacterQuestBegin(Runtime, Character, Packet->QuestID, Packet->SlotID)) {
		Response->Result = 1;
	}
	else {
		(true);
	}

	return SocketSend(Socket, Connection, Response);

error:
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(QUEST_CLEAR) {
	S2C_DATA_QUEST_CLEAR* Response = PacketBufferInit(Connection->PacketBuffer, S2C, QUEST_CLEAR);
	Response->Result = 0;

	if (!Character) goto error;

	Int32 SlotCount = (Packet->Length - sizeof(C2S_DATA_QUEST_CLEAR)) / sizeof(UInt16);
	UInt64 RewardExp;
	UInt32 RewardSkillExp;
	if (RTCharacterQuestClear(Runtime, Character, Packet->QuestIndex, Packet->QuestSlotIndex, SlotCount, Packet->SlotIndex, &RewardExp, &RewardSkillExp)) {
		Response->Result = 1;
		Response->ResultType = S2C_DATA_QUEST_CLEAR_RESULT_TYPE_SUCCESS;
		Response->RewardExp = RewardExp;
	}
	else {
		(true);
	}

	SocketSend(Socket, Connection, Response);
	/*
	RTQuestDataRef QuestData = RTRuntimeGetQuestByIndex(Runtime, Packet->QuestID);
	
	S2C_DATA_NFY_EVENT_PASS_PROGRESS* Notification1 = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_EVENT_PASS_PROGRESS);
	Notification1->GroupID = 3;
	Notification1->MissionID = 12;
	Notification1->Progress = 2;
	SocketSend(Socket, Connection, Notification1);

	S2C_DATA_NFY_EVENT_PASS_RESUME* Notification2 = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_EVENT_PASS_RESUME);
	Notification2->Unknown1 = 0;
	Notification2->Unknown2 = 100;
	Notification2->Unknown3 = 0;
	SocketSend(Socket, Connection, Notification2);
	*/
	return;

error:
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(QUEST_CANCEL) {
	S2C_DATA_QUEST_CANCEL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, QUEST_CANCEL);

	if (!RTCharacterQuestCancel(Runtime, Character, Packet->QuestID, Packet->SlotID)) goto error;

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_ACTION) {
	if (!Character) goto error;

	Bool Success = true;
	for (Int32 Index = 0; Index < Packet->ActionCount; Index++) {
		Success &= RTCharacterQuestAction(
			Runtime,
			Character,
			Packet->QuestID,
			Packet->NpcSetID,
			Packet->SlotIndex,
			Packet->Actions[Index].ActionIndex,
			Packet->Actions[Index].SlotIndex
		);
	}

	if (!Success) goto error;

	RTQuestSlotRef QuestSlot = &Character->QuestSlotInfo.QuestSlot[Packet->SlotIndex];

	S2C_DATA_QUEST_ACTION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, QUEST_ACTION);
	Response->QuestID = Packet->QuestID;
	Response->NpcFlags = 0;

	for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index++) {
		Response->NpcFlags |= 1 << Index;
	}
    
	// TODO: Set correct npc set id
	Response->NpcSetID = Packet->NpcSetID;

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPDATE_QUEST_LIST) {
	RTCharacterSetQuestDisplayNotice(
		Runtime,
		Character,
		Packet->QuestID,
		Packet->SlotIndex,
		Packet->DisplayNotice,
		Packet->DisplayOpenNotice
	);
}

CLIENT_PROCEDURE_BINDING(PARTY_QUEST_BEGIN) {
	if (!Character) goto error;

	S2C_DATA_PARTY_QUEST_BEGIN* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_QUEST_BEGIN);

	if (!RTCharacterPartyQuestBegin(Runtime, Character, Packet->QuestID, Packet->QuestSlotIndex, Packet->QuestItemSlotIndex)) {
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);

	if (!Response->Result) {
		S2C_DATA_NFY_PARTY_QUEST_BEGIN* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_PARTY_QUEST_BEGIN);
		Notification->QuestID = Packet->QuestID;
		Notification->QuestSlotIndex = Packet->QuestSlotIndex;
		BroadcastToParty(Context, Character->PartyID, Notification);
	}

	return;

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_QUEST_CLOSE) {
	S2C_DATA_PARTY_QUEST_CLOSE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_QUEST_CLOSE);
	Response->Result = 1;

	if (!Character) goto error;
	if (RTEntityIsNull(Character->PartyID)) goto error;
	if (Packet->QuestSlotIndex < 0 || Packet->QuestSlotIndex >= RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT) goto error;

	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	assert(Party);

	RTQuestSlotRef QuestSlot = &Party->QuestSlot[Packet->QuestSlotIndex];
	if (QuestSlot->QuestIndex != Packet->QuestID) goto error;

	RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
	if (!Quest) goto error;

	Int32 TailLength = 0;
	TailLength += Quest->MissionItemCount * sizeof(UInt16);

	Int32 RewardItemSetID = Quest->Reward[RUNTIME_QUEST_REWARD_ITEM_SET];
	if (RewardItemSetID > 0) {
		TailLength += sizeof(UInt16) * 2;
	}

	if (Packet->Length != sizeof(C2S_DATA_PARTY_QUEST_CLOSE) + TailLength) {
		LogMessageFormat("Packet length error for quest(%d) close!", Quest->ID);
		goto error;
	}
	
	Int32 Offset = 0;
	UInt16 InventorySlotIndices[3] = { 0, 0, 0 };
	for (Int32 Index = 0; Index < Quest->MissionItemCount; Index += 1) {
		InventorySlotIndices[Index] = *((UInt16*)&Packet->Data[Offset]);
		Offset += sizeof(UInt16);
	}

	UInt16 RewardItemIndex = 0;
	UInt16 RewardInventorySlotIndex = 0;
	if (RewardItemSetID > 0) {
		RewardItemIndex = *((UInt16*)&Packet->Data[Offset]);
		Offset += sizeof(UInt16);

		RewardInventorySlotIndex = *((UInt16*)&Packet->Data[Offset]);
		Offset += sizeof(UInt16);
	}

	Bool Success = RTCharacterPartyQuestClear(
		Runtime,
		Character, 
		Packet->QuestID, 
		Packet->QuestSlotIndex, 
		InventorySlotIndices,
		RewardItemIndex,
		RewardInventorySlotIndex
	);

	if (Success) {
		Response->Result = 0;
	}

	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_PARTY_QUEST_CLOSE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_PARTY_QUEST_CLOSE);
	Notification->QuestID = Packet->QuestID;
	Notification->QuestSlotIndex = Packet->QuestSlotIndex;

	if (RewardItemSetID > 0) {
		PacketBufferAppendValue(Context->ClientSocket->PacketBuffer, UInt16, RewardInventorySlotIndex);
	}

	return BroadcastToParty(Context, Character->PartyID, Notification);

error:
	Response->Result = 1;
	return SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(PARTY_QUEST_ACTION) {
	if (!Character) goto error;

	Bool Success = true;
	for (Int32 Index = 0; Index < Packet->ActionCount; Index += 1) {
		Success &= RTCharacterPartyQuestAction(
			Runtime,
			Character,
			Packet->QuestID,
			Packet->NpcSetID,
			Packet->QuestSlotIndex,
			Packet->Actions[Index].ActionIndex,
			Packet->Actions[Index].SlotIndex
		);
	}

	if (!Success) goto error;

	assert(!RTEntityIsNull(Character->PartyID));
	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	assert(Party);
	RTQuestSlotRef QuestSlot = &Party->QuestSlot[Packet->QuestSlotIndex];
	assert(QuestSlot);

	S2C_DATA_PARTY_QUEST_ACTION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_QUEST_ACTION);
	Response->QuestSlotIndex = Packet->QuestSlotIndex;
	Response->QuestID = Packet->QuestID;
	Response->NpcSetID = Packet->NpcSetID;

	for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index += 1) {
		Response->NpcFlags |= 1 << Index;
	}

	// TODO: Set correct npc set id
	Response->NpcSetID = Packet->NpcSetID;

	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_PARTY_QUEST_ACTION* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_PARTY_QUEST_ACTION);
	Notification->QuestSlotIndex = Response->QuestSlotIndex;
	Notification->QuestID = Response->QuestID;
	Notification->NpcFlags = Response->NpcFlags;
	Notification->NpcSetID = Response->NpcSetID;
	return BroadcastToParty(Context, Character->PartyID, Notification);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PARTY_QUEST_LOOT_ITEM) {
	if (!Character) goto error;
	if (RTEntityIsNull(Character->PartyID)) goto error;

	RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
	if (!Party) goto error;
	if (Packet->QuestItemSlotIndex < 0 || Packet->QuestItemSlotIndex >= RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT) goto error;

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
	if (!World) goto error;

	RTWorldItemRef Item = RTWorldGetItem(Runtime, World, Packet->Entity, Packet->UniqueKey);
	if (!Item) goto error;

	// TODO: We should always check the masking based in ItemData->ItemType...
	RTItemSlotRef ItemSlot = &Party->Inventory[Packet->QuestItemSlotIndex];
	if (!ItemSlot->Item.Serial) {
		ItemSlot->Item = Item->Item;
		ItemSlot->ItemOptions = Item->ItemOptions;
	}
	else {
		if (ItemSlot->Item.PartyQuestItemID != Item->Item.PartyQuestItemID ||
			RTQuestItemGetOptions(ItemSlot->ItemOptions) != RTQuestItemGetOptions(Item->ItemOptions)) goto error;

		UInt64 ItemOptions = RTQuestItemGetOptions(ItemSlot->ItemOptions);
		UInt64 ItemCount = RTQuestItemGetCount(ItemSlot->ItemOptions);
		ItemCount += RTQuestItemGetCount(Item->ItemOptions);
		ItemSlot->ItemOptions = RTQuestItemOptions(ItemOptions, ItemCount);
	}

	RTWorldDespawnItem(Runtime, World, Item);

	S2C_DATA_PARTY_QUEST_LOOT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, PARTY_QUEST_LOOT_ITEM);
	Response->Result = 1;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOptions = ItemSlot->ItemOptions;
	Response->QuestItemSlotIndex = Packet->QuestItemSlotIndex;
	Response->ItemDuration = ItemSlot->ItemDuration.Serial;
	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_PARTY_QUEST_LOOT_ITEM* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_PARTY_QUEST_LOOT_ITEM);
	Notification->ItemID = ItemSlot->Item.Serial;
	Notification->ItemOptions = ItemSlot->ItemOptions;
	Notification->QuestItemSlotIndex = Packet->QuestItemSlotIndex;
	Notification->ItemDuration = ItemSlot->ItemDuration.Serial;
	return BroadcastToParty(Context, Character->PartyID, Notification);

error:
	return SocketDisconnect(Socket, Connection);
}
