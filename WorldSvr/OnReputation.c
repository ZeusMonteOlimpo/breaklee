#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(REPUTATION_CHANGE) {
	if (!Character) goto error;

    // TODO: Add implementation
    return;

error:
	return SocketDisconnect(Socket, Connection);
}
