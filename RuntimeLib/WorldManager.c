#include "Runtime.h"
#include "WorldManager.h"

RTWorldManagerRef RTWorldManagerCreate(
    RTRuntimeRef Runtime,
    Index MaxWorldDataCount,
    Index MaxGlobalWorldContextCount,
    Index MaxPartyWorldContextCount,
    Index MaxCharacterCount
) {
    RTWorldManagerRef WorldManager = (RTWorldManagerRef)AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTWorldManager));
    if (!WorldManager) FatalError("Memory allocation failed");

    WorldManager->Allocator = Runtime->Allocator;
    WorldManager->Runtime = Runtime;
    WorldManager->MaxWorldDataCount = MaxWorldDataCount;
    WorldManager->MaxGlobalWorldContextCount = MaxGlobalWorldContextCount;
    WorldManager->MaxPartyWorldContextCount = MaxPartyWorldContextCount;
    WorldManager->MaxCharacterCount = MaxCharacterCount;
    WorldManager->WorldDataPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldData),
        MaxWorldDataCount
    );
    WorldManager->GlobalWorldContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldContext),
        MaxGlobalWorldContextCount
    );
    WorldManager->PartyWorldContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldContext),
        MaxPartyWorldContextCount
    );
    WorldManager->CharacterContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacter),
        MaxCharacterCount
    );
    WorldManager->IndexToGlobalWorldContextPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        MaxGlobalWorldContextCount
    );
    WorldManager->PartyToWorldContextPoolIndex = EntityDictionaryCreate(
        Runtime->Allocator,
        MaxPartyWorldContextCount
    );
    WorldManager->IndexToCharacterContextPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        MaxCharacterCount
    );

    // NOTE: WorldIndex 0 is reserved for an unused world context to preserve it for null checks
    Index NullIndex = 0;
    MemoryPoolReserve(WorldManager->GlobalWorldContextPool, NullIndex);
    MemoryPoolReserve(WorldManager->PartyWorldContextPool, NullIndex);
    MemoryPoolReserve(WorldManager->CharacterContextPool, NullIndex);
    
    return WorldManager;
}

Void RTWorldManagerDestroy(
    RTWorldManagerRef WorldManager
) {
    for (Index WorldIndex = 0; WorldIndex < WorldManager->MaxGlobalWorldContextCount; WorldIndex += 1) {
        if (!MemoryPoolIsReserved(WorldManager->GlobalWorldContextPool, WorldIndex)) continue;
        
        RTWorldContextDestroyGlobal(WorldManager, WorldIndex);
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldManager->PartyToWorldContextPoolIndex);
    while (Iterator.Key) {
        RTEntityID Party = *(RTEntityID*)Iterator.Key;
        RTWorldContextDestroyParty(WorldManager, Party);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    MemoryPoolDestroy(WorldManager->WorldDataPool);
    MemoryPoolDestroy(WorldManager->GlobalWorldContextPool);
    MemoryPoolDestroy(WorldManager->PartyWorldContextPool);
    MemoryPoolDestroy(WorldManager->CharacterContextPool);
    DictionaryDestroy(WorldManager->IndexToGlobalWorldContextPoolIndex);
    DictionaryDestroy(WorldManager->PartyToWorldContextPoolIndex);
    DictionaryDestroy(WorldManager->IndexToCharacterContextPoolIndex);
    AllocatorDeallocate(WorldManager->Allocator, WorldManager);
}

Void RTWorldManagerUpdate(
    RTWorldManagerRef WorldManager
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldManager->IndexToGlobalWorldContextPoolIndex);
    while (Iterator.Key) {
        Index WorldIndex = *(Index*)Iterator.Key;
        RTWorldContextRef WorldContext = RTWorldContextGetGlobal(WorldManager, WorldIndex);
        if (WorldContext->Active) RTWorldContextUpdate(WorldContext);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Iterator = DictionaryGetKeyIterator(WorldManager->PartyToWorldContextPoolIndex);
    while (Iterator.Key) {
        RTEntityID Party = *(RTEntityID*)Iterator.Key;
        RTWorldContextRef WorldContext = RTWorldContextGetParty(WorldManager, Party);
        if (WorldContext->Active) RTWorldContextUpdate(WorldContext);

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
}

RTWorldDataRef RTWorldDataCreate(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    return (RTWorldDataRef)MemoryPoolReserve(WorldManager->WorldDataPool, WorldIndex);
}

Bool RTWorldDataExists(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    return MemoryPoolIsReserved(WorldManager->WorldDataPool, WorldIndex);
}

RTWorldDataRef RTWorldDataGet(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    return (RTWorldDataRef)MemoryPoolFetch(WorldManager->WorldDataPool, WorldIndex);
}

RTWorldContextRef RTWorldContextGet(
    RTWorldManagerRef WorldManager,
    Index WorldIndex,
    Index WorldInstanceIndex
) {
    RTWorldDataRef WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    if (WorldData->Type == RUNTIME_WORLD_TYPE_GLOBAL) {
        return RTWorldContextGetGlobal(WorldManager, WorldIndex);
    }

    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->PartyWorldContextPool, WorldInstanceIndex);
}

RTWorldContextRef RTWorldContextCreateGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    assert(!DictionaryLookup(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex));

    Index WorldPoolIndex = 0;
    RTWorldContextRef WorldContext = (RTWorldContextRef)MemoryPoolReserveNext(WorldManager->GlobalWorldContextPool, &WorldPoolIndex);
    assert(WorldContext);
    WorldContext->WorldManager = WorldManager;
    WorldContext->WorldPoolIndex = WorldIndex;
    WorldContext->WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    WorldContext->DungeonIndex = 0;
    WorldContext->Party = kEntityIDNull;
    WorldContext->Seed = (Int32)PlatformGetTickCount();
    WorldContext->Active = false;
    WorldContext->Closed = false;
    WorldContext->DungeonTimeout = 0;
    WorldContext->MobPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMob), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->ItemPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTWorldItem), RUNTIME_MEMORY_MAX_ITEM_COUNT);
    WorldContext->EntityToMob = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToItem = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_ITEM_COUNT);
   
    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkInitialize(WorldManager->Allocator, &WorldContext->Chunks[ChunkIndex]);
    }

    DictionaryInsert(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex, &WorldPoolIndex, sizeof(Index));

    if (WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        WorldContext->Active = true;
    }

    return WorldContext;
}

RTWorldContextRef RTWorldContextGetGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    Index* WorldPoolIndex = (Index*)DictionaryLookup(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex);
    if (!WorldPoolIndex) return NULL;
    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->GlobalWorldContextPool, *WorldPoolIndex);
}

Void RTWorldContextDestroyGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
) {
    RTWorldContextRef WorldContext = RTWorldContextGetGlobal(WorldManager, WorldIndex);

    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkDeinitialize(&WorldContext->Chunks[ChunkIndex]);
    }

    MemoryPoolDestroy(WorldContext->MobPool);
    MemoryPoolDestroy(WorldContext->ItemPool);
    DictionaryDestroy(WorldContext->EntityToMob);
    DictionaryDestroy(WorldContext->EntityToItem);
    MemoryPoolRelease(WorldManager->GlobalWorldContextPool, WorldContext->WorldData->WorldIndex);
}

RTWorldContextRef RTWorldContextCreateParty(
    RTWorldManagerRef WorldManager,
    Index WorldIndex,
    Index DungeonIndex,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));
    assert(!DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party));

    Index WorldPoolIndex = 0;
    RTWorldContextRef WorldContext = (RTWorldContextRef)MemoryPoolReserveNext(WorldManager->PartyWorldContextPool, &WorldPoolIndex);
    WorldContext->WorldManager = WorldManager;
    WorldContext->WorldPoolIndex = WorldPoolIndex;
    WorldContext->WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    WorldContext->DungeonIndex = DungeonIndex;
    WorldContext->Party = Party;
    WorldContext->Seed = (Int32)PlatformGetTickCount();
    WorldContext->Active = false;
    WorldContext->Closed = false;
    WorldContext->DungeonTimeout = 0;
    WorldContext->MobPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMob), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->ItemPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTWorldItem), RUNTIME_MEMORY_MAX_ITEM_COUNT);
    WorldContext->EntityToMob = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToItem = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_ITEM_COUNT);
    
    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkInitialize(WorldManager->Allocator, &WorldContext->Chunks[ChunkIndex]);
    }
    
    DictionaryInsert(WorldManager->PartyToWorldContextPoolIndex, &Party, &WorldPoolIndex, sizeof(Index));
    return WorldContext;
}

Bool RTWorldContextPartyIsFull(
    RTWorldManagerRef WorldManager
) {
    return MemoryPoolIsFull(WorldManager->PartyWorldContextPool);
}

Bool RTWorldContextHasParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    if (RTEntityIsNull(Party)) return false;

    Index* WorldPoolIndex = (Index*)DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party);
    return (WorldPoolIndex != NULL);
}

RTWorldContextRef RTWorldContextGetParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));
    Index *WorldPoolIndex = (Index*)DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party);
    if (!WorldPoolIndex) return NULL;
    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->PartyWorldContextPool, *WorldPoolIndex);
}

Void RTWorldContextDestroyParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));

    RTWorldContextRef WorldContext = RTWorldContextGetParty(WorldManager, Party);

    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkDeinitialize(&WorldContext->Chunks[ChunkIndex]);
    }

    MemoryPoolDestroy(WorldContext->MobPool);
    MemoryPoolDestroy(WorldContext->ItemPool);
    MemoryPoolRelease(WorldManager->PartyWorldContextPool, WorldContext->WorldPoolIndex);
    DictionaryRemove(WorldManager->PartyToWorldContextPoolIndex, &Party);
}

RTCharacterRef RTWorldManagerCreateCharacter(
    RTWorldManagerRef WorldManager,
    Index CharacterIndex
) {
    assert(!DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex));

    Index CharacterPoolIndex = 0;
    RTCharacterRef Character = (RTCharacterRef)MemoryPoolReserveNext(WorldManager->CharacterContextPool, &CharacterPoolIndex);
    Character->CharacterIndex = CharacterIndex;
    Character->ID.EntityIndex = (UInt16)CharacterPoolIndex;
    Character->ID.EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
    DictionaryInsert(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex, &CharacterPoolIndex, sizeof(Index));
    return Character;
}

Bool RTWorldManagerCharacterPoolIsFull(
    RTWorldManagerRef WorldManager
) {
    return MemoryPoolIsFull(WorldManager->CharacterContextPool);
}

Bool RTWorldManagerHasCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
) {
    if (RTEntityIsNull(Entity)) return false;

    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    return MemoryPoolIsReserved(WorldManager->CharacterContextPool, Entity.EntityIndex);
}

RTCharacterRef RTWorldManagerGetCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
) {
    assert(!RTEntityIsNull(Entity));
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    return (RTCharacterRef)MemoryPoolFetch(WorldManager->CharacterContextPool, Entity.EntityIndex);
}

RTCharacterRef RTWorldManagerGetCharacterByIndex(
    RTWorldManagerRef WorldManager,
    Index CharacterIndex
) {
    Index* CharacterPoolIndex = DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
    if (!CharacterPoolIndex) return NULL;

    return (RTCharacterRef)MemoryPoolFetch(WorldManager->CharacterContextPool, *CharacterPoolIndex);
}

Void RTWorldManagerDestroyCharacter(
    RTWorldManagerRef WorldManager,
    Index CharacterIndex
) {
    Index* CharacterPoolIndex = DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
    assert(CharacterPoolIndex);

    MemoryPoolRelease(WorldManager->CharacterContextPool, *CharacterPoolIndex);
    DictionaryRemove(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
}
