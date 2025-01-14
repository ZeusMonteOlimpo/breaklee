#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SERVER_ENVIRONMENT) {
	if (!(Client->Flags & CLIENT_FLAGS_VERSION_CHECKED)) {
		return SocketDisconnect(Socket, Connection);
	}

	// TODO: Add last login based captcha
	/*
	Int32 UsernameLength = strlen(Packet->Username);
	if (UsernameLength < 1 || UsernameLength > MAX_USERNAME_LENGTH) {
		return SocketDisconnect(Socket, Connection);
	}
	memset(Client->Username, 0, sizeof(Client->Username));
	memcpy(Client->Username, Packet->Username, UsernameLength);
	IPC_DATA_REQAUTHENTICATE* Request = PacketBufferInitExtended(IPC_DATA_REQAUTHENTICATE);
	Request->ConnectionID = Connection->ID;
	memcpy(Request->Username, Packet->Username, UsernameLength);
	SocketSendAll(Server->DataSocket, Request);
}

DATA_PROCEDURE_BINDING(OnDataAccountInfo, IPC_DATA_ACCOUNTINFO, IPC_DATA_ACKACCOUNTINFO) {
	*/
	S2C_DATA_SERVER_ENVIRONMENT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SERVER_ENVIRONMENT);
	// TODO: Add support for image authentication
	Client->Flags |= CLIENT_FLAGS_USERNAME_CHECKED;
	SocketSend(Socket, Connection, Response);
}
