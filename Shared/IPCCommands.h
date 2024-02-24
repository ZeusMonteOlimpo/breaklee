#ifndef IPC_AUTH_COMMAND
#define IPC_AUTH_COMMAND(__NAME__, __VALUE__)
#endif

#ifndef IPC_WORLD_COMMAND
#define IPC_WORLD_COMMAND(__NAME__, __VALUE__)
#endif

IPC_AUTH_COMMAND(IPC_AUTH_REQCONNECT, 1001)
IPC_AUTH_COMMAND(IPC_AUTH_ACKCONNECT, 1002)
IPC_AUTH_COMMAND(IPC_AUTH_NFYWORLDLIST, 1003)
IPC_AUTH_COMMAND(IPC_AUTH_VERIFYLINKS, 1004)
IPC_AUTH_COMMAND(IPC_AUTH_VERIFYRESULT, 1005)
IPC_AUTH_COMMAND(IPC_AUTH_REQVERIFYPASSWORD, 1006)
IPC_AUTH_COMMAND(IPC_AUTH_ACKVERIFYPASSWORD, 1007)

IPC_WORLD_COMMAND(IPC_WORLD_REQCONNECT, 2001)
IPC_WORLD_COMMAND(IPC_WORLD_ACKCONNECT, 2002)
IPC_WORLD_COMMAND(IPC_WORLD_NFYUSERLIST, 2003)
IPC_WORLD_COMMAND(IPC_WORLD_VERIFYLINKS, 2004)
IPC_WORLD_COMMAND(IPC_WORLD_VERIFYRESULT, 2005)
IPC_WORLD_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_SESSION_DATA, 2006)
IPC_WORLD_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA, 2007)
IPC_WORLD_COMMAND(IPC_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA, 2008)
IPC_WORLD_COMMAND(IPC_WORLD_REQGETCHARACTERS, 2009)
IPC_WORLD_COMMAND(IPC_WORLD_ACKGETCHARACTERS, 2010)
IPC_WORLD_COMMAND(IPC_WORLD_REQPREMIUMSERVICE, 2011)
IPC_WORLD_COMMAND(IPC_WORLD_ACKPREMIUMSERVICE, 2012)
IPC_WORLD_COMMAND(IPC_WORLD_REQCREATECHARACTER, 2013)
IPC_WORLD_COMMAND(IPC_WORLD_ACKCREATECHARACTER, 2014)
IPC_WORLD_COMMAND(IPC_WORLD_REQGETCHARACTER, 2015)
IPC_WORLD_COMMAND(IPC_WORLD_ACKGETCHARACTER, 2016)
IPC_WORLD_COMMAND(IPC_WORLD_REQDBSYNC, 2017)
IPC_WORLD_COMMAND(IPC_WORLD_ACKDBSYNC, 2018)
IPC_WORLD_COMMAND(IPC_WORLD_REQVERIFYPASSWORD, 2019)
IPC_WORLD_COMMAND(IPC_WORLD_ACKVERIFYPASSWORD, 2020)

#undef IPC_AUTH_COMMAND
#undef IPC_WORLD_COMMAND