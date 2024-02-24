#pragma once

#include "Base.h"
#include "Constants.h"

#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_QUEST_ACTION_TYPE_GIVE,
    RUNTIME_QUEST_ACTION_TYPE_TAKE,
    RUNTIME_QUEST_ACTION_TYPE_TALK,
    RUNTIME_QUEST_ACTION_TYPE_GIVE_MAP_CODE,
    RUNTIME_QUEST_ACTION_TYPE_GIVE_WARP_CODE,
    RUNTIME_QUEST_ACTION_TYPE_BEGIN,
    RUNTIME_QUEST_ACTION_TYPE_END,
    RUNTIME_QUEST_ACTION_TYPE_GIVE_ALZ,
    RUNTIME_QUEST_ACTION_TYPE_TAKE_ALZ,
    RUNTIME_QUEST_ACTION_TYPE_TRIGGER,
};

enum {
    RUNTIME_QUEST_TYPE_BATTLE_RANK_UP = 4,
};

enum {
    RUNTIME_QUEST_REWARD_EXP = 0,
    RUNTIME_QUEST_REWARD_SKILL = 2,
    RUNTIME_QUEST_REWARD_ITEM_SET = 3,
    RUNTIME_QUEST_REWARD_HONOR_POINT = 4,
    RUNTIME_QUEST_REWARD_SKILL_EXP = 5,
};

enum {
    RUNTIME_QUEST_RANK_TYPE_SWORD = 1,
    RUNTIME_QUEST_RANK_TYPE_MAGIC = 2,
};

struct _RTQuestConditionData {
    Int32 Level;
    Int32 Class[2];
    Int32 Item[3];
    Int32 MaxLevel;
    Int32 Rank;
    Int32 RankType;
    Int32 Skill[2];
    Int32 MinHonorRank;
    Int32 MaxHonorRank;
    Int32 RelatedQuestID;
    Int32 NationType;
    Int32 MinOverlordLevel;
    Int32 MaxOverlordLevel;
};

struct _RTQuestNpcData {
    Int32 Index;
    Int32 WorldID;
    Int32 NpcID;
    Int32 NpcActionOrder;
    Int32 ActionType;
    Int32 Value[3];
    Int32 Notification[2];
};

struct _RTQuestNpcSetData {
    Int32 ID;
    Int32 Count;
    struct _RTQuestNpcData Npcs[RUNTIME_MEMORY_MAX_QUEST_NPC_COUNT];
};

struct _RTQuestMissionData {
    Int32 Type;
    Int32 Order;
    Int32 NotifyOrder;
    Int32 Value[3];
    Int32 QuestIcon;
    Int32 Location1[3];
    Int32 Location2[3];
    Int32 Location3[3];
};

struct _RTQuestData {
    Int32 Index;
    Int32 ID;
    Int32 Type;
    Int32 Party;
    Int32 MissionOrder;
    Int32 MissionSet;
    Int32 MissionMobCount;
    Int32 MissionItemCount;
    Int32 MissionDungeonCount;
    Int32 DeleteType;
    Int32 DailyCount;
    struct _RTQuestConditionData Condition;
    Int32 Reward[9];
    Int32 QuestBeginNpcIndex;
    struct _RTQuestNpcSetData NpcSet;
    struct _RTQuestMissionData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTQuestMissionData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTQuestMissionData MissionDungeons[RUNTIME_MAX_QUEST_COUNTER_COUNT];
};

struct _RTQuestRewardItemData {
    Int32 BattleStyleIndex;
    Int32 Index;
    UInt32 ItemID[2];
    Int32 ItemDuration;
};

struct _RTQuestRewardItemSetData {
    Int32 ID;
    Int32 Count;
    struct _RTQuestRewardItemData Items[RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_DATA_COUNT];
};

struct _RTQuestSlot {
    UInt16 QuestIndex;
    UInt16 NpcActionIndex;
    UInt8 DisplayNotice;
    UInt8 DisplayOpenNotice;
    UInt8 Counter[RUNTIME_MAX_QUEST_COUNTER_COUNT];
};

struct _RTCharacterQuestSlotInfo {
    struct _RTQuestSlot QuestSlot[RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT];
};

struct _RTCharacterQuestFlagInfo {
    UInt8 Flags[RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT];
};

#pragma pack(pop)

Void RTCharacterQuestFlagClear(
    RTCharacterRef Character,
    Int32 QuestIndex
);

Void RTCharacterQuestFlagSet(
    RTCharacterRef Character,
    Int32 QuestIndex
);

Bool RTCharacterQuestFlagIsSet(
    RTCharacterRef Character,
    Int32 QuestIndex
);

Void RTCharacterDungeonQuestFlagClear(
    RTCharacterRef Character,
    Int32 DungeonIndex
);

Void RTCharacterDungeonQuestFlagSet(
    RTCharacterRef Character,
    Int32 DungeonIndex
);

Bool RTCharacterDungeonQuestFlagIsSet(
    RTCharacterRef Character,
    Int32 DungeonIndex
);

Bool RTCharacterQuestBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 QuestSlotIndex,
    Int32 SlotCount,
    UInt16* SlotIndex,
    UInt64* ResultExp,
    UInt32* ResultSkillExp
);

Bool RTCharacterQuestCancel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterQuestAction(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 NpcSetIndex,
    Int32 SlotIndex,
    Int32 ActionIndex,
    Int32 ActionSlotIndex
);

Bool RTCharacterSetQuestDisplayNotice(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 QuestSlotIndex,
    UInt8 DisplayNotice,
    UInt8 DisplayOpenNotice
);

Bool RTCharacterIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Index MobSpeciesIndex
);

Bool RTCharacterUpdateQuestItemCounter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTItem Item,
    UInt64 ItemOptions
);

Bool RTCharacterHasQuestItemCounter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTItem Item,
    UInt64 ItemOptions
);

Bool RTCharacterHasQuestDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 DungeonID
);

Void RTPartyQuestFlagClear(
    RTPartyRef Party,
    Int32 QuestIndex
);

Void RTPartyQuestFlagSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTPartyQuestFlagIsSet(
    RTPartyRef Party,
    Int32 QuestIndex
);

Bool RTCharacterPartyQuestBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex,
    Int16 QuestItemSlotIndex
);

Bool RTCharacterPartyQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 QuestSlotIndex,
    UInt16 InventorySlotIndices[RUNTIME_MAX_QUEST_COUNTER_COUNT],
    Int32 RewardItemIndex,
    UInt16 RewardItemSlotIndex
);

Bool RTCharacterPartyQuestCancel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 SlotIndex
);

Bool RTCharacterPartyQuestAction(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex,
    Int32 NpcSetIndex,
    Int32 SlotIndex,
    Int32 ActionIndex,
    Int32 ActionSlotIndex
);

Bool RTPartyIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTEntityID PartyID,
    Index MobSpeciesIndex
);

Bool RTPartyHasQuestItemCounter(
    RTRuntimeRef Runtime,
    RTPartyRef Party,
    RTItem Item,
    UInt64 ItemOptions
);

Bool RTCharacterHasPartyQuestDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 DungeonID
);

EXTERN_C_END