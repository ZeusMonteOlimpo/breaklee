#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CONNECT) {
    S2C_DATA_CONNECT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CONNECT);
    Response->XorKey = (UInt32)rand();
    Response->AuthKey = (UInt32)rand();
    Response->ConnectionID = (UInt16)Connection->ID;
    Response->XorKeyIndex = (UInt16)rand() % KEYCHAIN_XORKEY_COUNT;
    Response->Unknown1 = 2;
    Response->Unknown2 = 4;
    Client->Flags |= CLIENT_FLAGS_CONNECTED;
    SocketConnectionKeychainSeed(Socket, Connection, Response->XorKey, Response->XorKeyIndex);
    SocketSend(Socket, Connection, Response);
}
