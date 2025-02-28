#include "Mob.h"
#include "Runtime.h"
#include "Script.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static Int32 _DebugWorldSpawnMob(lua_State* State);

struct _RTScript {
    Index PoolIndex;
    lua_State* State;
};

struct _RTScriptManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Index MaxScriptCount;
    MemoryPoolRef ScriptPool;
    DictionaryRef ScriptTable;
};

#define SCRIPT_STRUCTURE_BINDING(__TYPE__, __ACCESSOR__, __ACCESSOR_TYPE__, __NAME__)     \
static Int32 SCRIPT_PROC_ ## __TYPE__ ## _ ## __NAME__ (lua_State* State) {               \
    struct _RT ## __TYPE__* __TYPE__ = (struct _RT ## __TYPE__*)lua_touserdata(State, 1); \
    lua_push ## __ACCESSOR_TYPE__ (State, __ACCESSOR__);                                  \
    Int32 ReturnValueCount = 1;                                                           \
    return ReturnValueCount;                                                              \
}
#include "MobAPI.h"

Void RTScriptBindMobAPI(
    RTScriptRef Script
) {
#define SCRIPT_STRUCTURE_BINDING(__TYPE__, __ACCESSOR__, __ACCESSOR_TYPE__, __NAME__) \
    lua_pushcfunction(Script->State, SCRIPT_PROC_ ## __TYPE__ ## _ ## __NAME__);      \
    lua_setglobal(Script->State, # __NAME__);
#include "MobAPI.h"
}

RTScriptManagerRef RTScriptManagerCreate(
    RTRuntimeRef Runtime,
    Index MaxScriptCount
) {
    RTScriptManagerRef ScriptManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTScriptManager));
    if (!ScriptManager) FatalError("Memory allocation failed!");

    ScriptManager->Allocator = Runtime->Allocator;
    ScriptManager->Runtime = Runtime;
    ScriptManager->MaxScriptCount = MaxScriptCount;
    ScriptManager->ScriptPool = MemoryPoolCreate(Runtime->Allocator, sizeof(struct _RTScript), MaxScriptCount);
    ScriptManager->ScriptTable = CStringDictionaryCreate(Runtime->Allocator, MaxScriptCount);

    return ScriptManager;
}

Void RTScriptManagerDestroy(
    RTScriptManagerRef ScriptManager
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(ScriptManager->ScriptTable);
    while (Iterator.Key) {
        Index MemoryPoolIndex = *(Index*)Iterator.Value;
        RTScriptRef Script = MemoryPoolFetch(ScriptManager->ScriptPool, MemoryPoolIndex);
        lua_close(Script->State);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    DictionaryDestroy(ScriptManager->ScriptTable);
    MemoryPoolDestroy(ScriptManager->ScriptPool);
    AllocatorDeallocate(ScriptManager->Allocator, ScriptManager);
}

RTScriptRef RTScriptManagerLoadScript(
    RTScriptManagerRef ScriptManager,
    CString FilePath
) {
    Index* ScriptPoolIndex = (Index*)DictionaryLookup(ScriptManager->ScriptTable, FilePath);
    if (ScriptPoolIndex) return (RTScriptRef)MemoryPoolFetch(ScriptManager->ScriptPool, *ScriptPoolIndex);

    Index MemoryPoolIndex = 0;
    RTScriptRef Script = (RTScriptRef)MemoryPoolReserveNext(ScriptManager->ScriptPool, &MemoryPoolIndex);
    DictionaryInsert(ScriptManager->ScriptTable, FilePath, &MemoryPoolIndex, sizeof(Index));

    Script->PoolIndex = MemoryPoolIndex;
    Script->State = luaL_newstate();
    if (!Script->State) FatalError("Lua: State creation failed!");

    luaL_openlibs(Script->State);

    RTScriptBindMobAPI(Script);

    lua_pushcfunction(Script->State, _DebugWorldSpawnMob);
    lua_setglobal(Script->State, "world_spawn_mob");
    
    if (luaL_loadfile(Script->State, FilePath) != LUA_OK) FatalErrorFormat("Lua: %s", lua_tostring(Script->State, -1));
    if (lua_pcall(Script->State, 0, 0, 0) != LUA_OK) FatalErrorFormat("Lua: %s", lua_tostring(Script->State, -1));

    return Script;
}

Bool RTScriptCall(
    RTScriptRef Script,
    const Char* Function,
    ...
) {
    Int32 Result = lua_getglobal(Script->State, Function);
    if (Result != LUA_TFUNCTION) {
        lua_pop(Script->State, 1);
        return false;
    }

    va_list ArgumentPointer;
    va_start(ArgumentPointer, Function);

    Void* Argument = NULL;
    Int32 ArgumentCount = 0;
    while (Argument = va_arg(ArgumentPointer, Void*)) {
        lua_pushlightuserdata(Script->State, Argument);
        ArgumentCount += 1;
    }

    va_end(ArgumentPointer);

    Result = lua_pcall(Script->State, ArgumentCount, 0, 0);
    if (Result != LUA_OK) {
        LogMessageFormat(LOG_LEVEL_ERROR, "Lua: %s", lua_tostring(Script->State, -1));
        return false;
    }

    return true;
}

// TODO: This is just for debugging for now..

static Int32 _DebugWorldSpawnMob(
    lua_State* State
) {
    RTRuntimeRef Runtime = lua_touserdata(State, 1);
    RTWorldContextRef WorldContext = lua_touserdata(State, 2);
    Int32 MobIndex = lua_tointeger(State, 3);
    Int32 X = lua_tointeger(State, 4);
    Int32 Y = lua_tointeger(State, 5);

    /*
    Index MemoryPoolIndex = 0;
    RTMobRef Minion = (RTMobRef)MemoryPoolReserveNext(WorldContext->MobPool, &MemoryPoolIndex);
    Minion->ID.EntityIndex = MemoryPoolIndex + 1;
    Minion->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
    Minion->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
    DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Index));

    Minion->Spawn.MobSpeciesIndex = MobSpeciesIndex;
    Minion->Spawn.AreaX = X;
    Minion->Spawn.AreaY = Y;
    Minion->Spawn.AreaWidth = 1;
    Minion->Spawn.AreaHeight = 1;

    Minion->SpeciesData = &Runtime->MobData[MobSpeciesIndex];
    Minion->IsInfiniteSpawn = true;
    Minion->IsPermanentDeath = false;
    Minion->RemainingSpawnCount = 0;
    RTWorldSpawnMob(Runtime, WorldContext, Minion);
    */

    RTEntityID MobID = { 0 };
    MobID.EntityIndex = MobIndex;
    MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
    MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
    RTMobRef Minion = RTWorldContextGetMob(WorldContext, MobID);
    if (Minion && !Minion->IsSpawned) {
        Minion->Spawn.AreaX = X;
        Minion->Spawn.AreaY = Y;
        RTWorldSpawnMob(Runtime, WorldContext, Minion);
    }

    return 0;
}

