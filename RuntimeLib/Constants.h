#pragma once

#include "Base.h"

#define RUNTIME_CHARACTER_MAX_NAME_LENGTH						16
#define RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT					255
#define RUNTIME_CHARACTER_MAX_QUICK_SLOT_COUNT					296
#define RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT					20
#define RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT					512
#define RUNTIME_CHARACTER_MAX_NORMAL_QUEST_FLAG_COUNT			128
#define RUNTIME_CHARACTER_MAX_QUEST_DUNGEON_FLAG_COUNT			640
#define RUNTIME_CHARACTER_QUEST_FLAG_SIZE						8
#define RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT					50
#define RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT				9
#define RUNTIME_CHARACTER_AXP_PER_LEVEL							10000000
#define RUNTIME_CHARACTER_MAX_ABILITY_POINT						700
#define RUNTIME_CHARACTER_ESSENCE_ABILITY_SLOT_COUNT			16
#define RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT		RUNTIME_CHARACTER_ESSENCE_ABILITY_SLOT_COUNT
#define RUNTIME_CHARACTER_MAX_OVERLORD_MASTERY_SLOT_COUNT		32
#define RUNTIME_CHARACTER_MAX_COLLECTION_SLOT_COUNT				512
#define RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT				6
#define RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT			5
#define RUNTIME_CHARACTER_MAX_FORCE_WING_RANK					6
#define RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_COUNT	2
#define RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_RATE		1000000
#define RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT	6
#define RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT		5
#define RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE		12
#define RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_TRAINING_COUNT	6
#define RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_SLOT_COUNT		(RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT * RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE * RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_TRAINING_COUNT)
#define RUNTIME_CHARACTER_FORCE_WING_LEVEL_TRAINING_POINT_COUNT 2
#define RUNTIME_CHARACTER_FORCE_WING_GRADE_TRAINING_POINT_COUNT 10
#define RUNTIME_CHARACTER_MAX_RAGE								10
#define RUNTIME_CHARACTER_MAX_BP								10000

#define RUNTIME_CHARACTER_MAX_NEWBIE_SUPPORT_SLOT_COUNT			128

#define RUNTIME_MAX_FORCE_EFFECT_CODE_COUNT						4

#define RUNTIME_ENCHANT_MAX_STAT_INCREASE_LEVEL					7
#define RUNTIME_ENCHANT_SLOPE_MULTIPLIER_LIMIT					4
#define RUNTIME_ENCHANT_SLOPE_DIVIDER							3

#define RUNTIME_MOB_FIND_COUNT_UNMOVABLE						100000000
#define RUNTIME_MOB_MAX_FIND_LEVEL_DIFFERENCE					100
#define RUNTIME_MOB_MAX_REACTION_DISTANCE						16
#define RUNTIME_MOB_MAX_ATTACK_RESULT_COUNT						8
#define RUNTIME_MOB_MAX_EVENT_COUNT								4

#define RUNTIME_INVENTORY_PAGE_WIDTH							8
#define RUNTIME_INVENTORY_PAGE_HEIGHT							8
#define RUNTIME_INVENTORY_PAGE_SIZE								(RUNTIME_INVENTORY_PAGE_WIDTH * RUNTIME_INVENTORY_PAGE_HEIGHT)
#define RUNTIME_INVENTORY_PAGE_COUNT							8
#define RUNTIME_INVENTORY_TOTAL_SIZE							(RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT)

#define RUNTIME_WAREHOUSE_PAGE_WIDTH							8
#define RUNTIME_WAREHOUSE_PAGE_HEIGHT							16
#define RUNTIME_WAREHOUSE_PAGE_SIZE								(RUNTIME_WAREHOUSE_PAGE_WIDTH * RUNTIME_WAREHOUSE_PAGE_HEIGHT)
#define RUNTIME_WAREHOUSE_PAGE_COUNT							8
#define RUNTIME_WAREHOUSE_TOTAL_SIZE							(RUNTIME_WAREHOUSE_PAGE_SIZE * RUNTIME_WAREHOUSE_PAGE_COUNT)

#define RUNTIME_ITEM_MASK_ID									(~(RUNTIME_ITEM_MASK_BINDING | RUNTIME_ITEM_MASK_LEVEL))
#define RUNTIME_ITEM_MAX_OPTION_COUNT							3

#define RUNTIME_MOVEMENT_MAX_WAYPOINT_COUNT						32
#define RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH					64
#define RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE					256
#define RUNTIME_MOVEMENT_SPEED_BASE								450
#define RUNTIME_MOVEMENT_SPEED_SCALE							100
#define RUNTIME_MOVEMENT_MAX_DISTANCE_IN_RANGE					4
#define RUNTIME_MOVEMENT_MAX_PATH_FIND_NODE_COUNT				1024

#define RUNTIME_NATION_COUNT						            3

#define RUNTIME_MAX_QUEST_COUNTER_COUNT					        3
#define RUNTIME_MAX_QUEST_DUNGEON_INDEX_COUNT					8

#define RUNTIME_SHOP_MAX_ITEM_COUNT                             128

#define RUNTIME_TRAINER_MAX_SKILL_COUNT                         512

#define RUNTIME_PARTY_MAX_MEMBER_COUNT							7
#define RUNTIME_PARTY_MAX_INVENTORY_SLOT_COUNT					8
#define RUNTIME_PARTY_MAX_QUEST_SLOT_COUNT						8

#define RUNTIME_WORLD_SIZE							            256
#define RUNTIME_WORLD_CHUNK_SIZE					            16
#define RUNTIME_WORLD_CHUNK_SIZE_EXPONENT			            4
#define RUNTIME_WORLD_CHUNK_COUNT								(RUNTIME_WORLD_SIZE / RUNTIME_WORLD_CHUNK_SIZE)
#define RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS						2
#define RUNTIME_WORLD_TILE_SIZE_EXPONENT                        4
#define RUNTIME_WORLD_MAX_NPC_COUNT				                16

#define RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT	                16
#define RUNTIME_DUNGEON_TRIGGER_MAX_MOB_COUNT                   4

#define RUNTIME_MEMORY_MAX_SCRIPT_COUNT							256
#define RUNTIME_MEMORY_MAX_ENTITY_COUNT				            (0xFFFF - 1)
#define RUNTIME_MEMORY_MAX_CHARACTER_COUNT			            0xFF
#define RUNTIME_MEMORY_MAX_CHARACTER_SKILL_DATA_COUNT           1024
#define RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT			            8192
#define RUNTIME_MEMORY_MAX_MOB_COUNT				            1024
#define RUNTIME_MEMORY_MAX_MOB_DATA_COUNT			            8192
#define RUNTIME_MEMOTY_MAX_MOB_SKILL_GROUP_COUNT                8
#define RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT                      8
#define RUNTIME_MEMORY_MAX_ITEM_COUNT				            (0xFF - 1)
#define RUNTIME_MEMORY_MAX_NPC_COUNT				            4096
#define RUNTIME_MEMORY_MAX_PARTY_COUNT							RUNTIME_MEMORY_MAX_CHARACTER_COUNT
#define RUNTIME_MEMORY_MAX_SHOP_DATA_COUNT			            128
#define RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT                   128
#define RUNTIME_MEMORY_MAX_QUEST_NPC_COUNT			            32
#define RUNTIME_MEMORY_MAX_QUEST_DATA_COUNT			            2048
#define RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_DATA_COUNT         64
#define RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_SET_DATA_COUNT     512
#define RUNTIME_MEMORY_MAX_WARP_INDEX_COUNT			            1024
#define RUNTIME_MEMORY_MAX_WARP_COUNT				            1024
#define RUNTIME_MEMORY_MAX_WORLD_DATA_COUNT                     256
#define RUNTIME_MEMORY_MAX_GLOBAL_WORLD_CONTEXT_COUNT		    256
#define RUNTIME_MEMORY_MAX_PARTY_WORLD_CONTEXT_COUNT		    256
#define RUNTIME_MEMORY_MAX_LEVEL_COUNT				            256
#define RUNTIME_MEMORY_MAX_SLOPE_DATA_COUNT                     8
#define RUNTIME_MEMORY_MAX_SLOPE_FORMULA_COUNT                  256
#define RUNTIME_MEMORY_MAX_SKILL_LEVEL_DATA_COUNT               32
#define RUNTIME_MEMORY_MAX_SKILL_RANK_DATA_COUNT                32
#define RUNTIME_MEMORY_MAX_BATTLE_RANK_DATA_COUNT               32
#define RUNTIME_MEMORY_MAX_HONOR_LEVEL_DATA_COUNT               64
#define RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT                512
#define RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT                  512
#define RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT                512
#define RUNTIME_MEMORY_MAX_DUNGEON_COUNT                        1024
#define RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT           2048
#define RUNTIME_MEMORY_DUNGEON_TRIGGER_DATA_COUNT			    256
#define RUNTIME_MEMORY_DUNGEON_TRIGGER_ACTION_DATA_COUNT	    2048
