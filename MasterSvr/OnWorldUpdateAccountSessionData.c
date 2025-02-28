#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldUpdateAccountSessionData, IPC_WORLD_UPDATE_ACCOUNT_SESSION_DATA, IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, &Account)) return;

	memcpy(Account.SessionIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH);
	Account.SessionTimeout = Packet->SessionTimeout;

	MasterDBUpdateAccountSession(Context->Database, &Account);
}
