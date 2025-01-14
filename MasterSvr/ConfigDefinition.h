#ifndef CONFIG_BEGIN
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER(UInt8, ServerID, "MasterSvr.ServerID", 1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38181)
CONFIG_PARAMETER(UInt32, MaxWorldCount, "MasterSvr.MaxWorldCount", 32)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(MasterDB)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterDB.Host", 127.0.0.1)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Username, "MasterDB.Username", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Password, "MasterDB.Password", root)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Database, "MasterDB.Database", master)
CONFIG_PARAMETER(UInt16, Port, "MasterDB.Port", 3312)
CONFIG_PARAMETER(Bool, AutoReconnect, "MasterDB.AutoReconnect", 1)
CONFIG_END(MasterDB)

CONFIG_BEGIN(AuthSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "AuthSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "AuthSvr.Port", 38180)
CONFIG_END(AuthSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x1FFFF)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
