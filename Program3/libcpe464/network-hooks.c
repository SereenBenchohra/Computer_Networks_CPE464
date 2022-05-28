// Modified to add ipv6 support 5/2017
// Modified to support Mac - added socketMod() to stop socketType

// ============================================================================
#include "networks/network-hooks.h"
#undef fork
#undef socket

#include "PacketManager.h"
#include "SettingsManager.h"

#include "MsgEvents/errorDrop.h"
#include "MsgEvents/errorFlipBits.h"
#include "MsgEvents/infoSeqNo.h"

#include "utils/dbg_print.h"
// ============================================================================
// Since we overrode send(to) functions, we need to undef here (before re-inc)

#undef bind
#undef select
#undef send
#undef sendto

#ifdef CPE464_OVERRIDE_RECV
    #undef recv
    #undef recvfrom
#endif
// ============================================================================
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <stdio.h>

#include <string.h>
#include <errno.h>
#include <time.h>

static int socketType = 0;
static int nextSeed = RANDOM_SEED;
static int childProcess = 0;

// ============================================================================
PacketManager   g_PktMgr;
SettingsManager g_SetsMgr(g_PktMgr);
// ============================================================================



int forkMod(void)
{
    int returnValue = 0;
	int tempNextSeed = nextSeed;
	
	nextSeed++;
	
	if ((returnValue = fork()) == 0)
	{
		// in child process - set a next random seed for child process
		g_SetsMgr.setUserMode_SeedRand(tempNextSeed);
		childProcess = 1;
	}
	
	return returnValue;
}

int socketMod(int domain, int type, int protocol)
{
	socketType = type;
	
	return(socket(domain, type, protocol));	
	
}

// ============================================================================
int bindMod(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    static int s_hasOverridden = 0;
    int port;

    char* env_port;
    env_port = getenv("CPE464_OVERRIDE_PORT");
    
	if (env_port && !s_hasOverridden)
    {
        s_hasOverridden = 1;
        port = atol(env_port);

        DBG_PRINT(DBG_LEVEL_WARN, "Port Override - %i\n", port);
		
		if (socketType == AF_INET6)
		{
			((struct sockaddr_in6*)addr)->sin6_port = htons(port);	
		}
		else
		{    
			((struct sockaddr_in*)addr)->sin_port = htons(port);
		}
    }

    int nResult = bind(sockfd, addr, addrlen);

	if (socketType == AF_INET6)
	{
		struct sockaddr_in6 addr_in6;
		socklen_t addr_in6_len;

		getsockname(sockfd, (sockaddr*)&addr_in6, &addr_in6_len);
		port = ntohs(addr_in6.sin6_port);
	}
	else
	{
		struct sockaddr_in addr_in;
		socklen_t addr_in_len;

		getsockname(sockfd, (sockaddr*)&addr_in, &addr_in_len);
		port = ntohs(addr_in.sin_port);
	}
	
    DBG_PRINT(DBG_LEVEL_DEBUG, "Port %i\n", port);

    return nResult;
}
// ============================================================================
int selectMod(int nfds,
              fd_set *readfds,
              fd_set *writefds,
              fd_set *exceptfds,
              struct timeval *timeout)
{
		struct timeval holdTimeValue;
		
	if (timeout != NULL)
	{
		holdTimeValue.tv_sec = timeout->tv_sec;
		holdTimeValue.tv_usec = timeout->tv_usec;
	}
		
	if (timeout == NULL )
	{
		DBG_PRINT(DBG_LEVEL_INFO, "Selected called with NULL Time value - not allowed in this program.\n");
	} 
	else if ((holdTimeValue.tv_sec != 0 && holdTimeValue.tv_sec != 1 && holdTimeValue.tv_sec != 10) || (holdTimeValue.tv_usec != 0))
	{
		DBG_PRINT(DBG_LEVEL_INFO, "Selected called with non standard value - was sec: %d usec: %d ", holdTimeValue.tv_sec, holdTimeValue.tv_usec);
		DBG_PRINT(DBG_LEVEL_INFO, "should be either 0, 1 or 10 seconds\n");
	}
	/*    if (timeout == NULL)
	{
		DBG_PRINT(DBG_LEVEL_INFO, "NULL timeout... infinite wait\n");
	}
	else
	{
		DBG_PRINT(DBG_LEVEL_INFO, "Secs %2li, uSecs %2li\n", timeout->tv_sec, timeout->tv_usec);
	}
	*/
	int nResult = select(nfds, readfds, writefds, exceptfds, timeout);
	// IF the select time was NULL it was a block on select() until data came in
	// Otherwise see if the user set a timeout value - only print message if the time
	// value was greater than 0 (meaning a timeout occured)
	if (nResult == 0 && timeout != NULL && (holdTimeValue.tv_sec != 0 || holdTimeValue.tv_usec != 0))
	{
		if (childProcess == 1)
		{
			DBG_PRINT(DBG_LEVEL_INFO, " Child process - ");
		}
		
		DBG_PRINT(DBG_LEVEL_INFO, "Select Timed Out - pid: %d", getpid());
		DBG_PRINT(DBG_LEVEL_INFO, " sec: %d usec: %d\n",holdTimeValue.tv_sec, holdTimeValue.tv_usec);
	}


	return nResult;
	}
// ============================================================================
int sendErr_init(double error_rate,
                 int drop_flag,
                 int flip_flag,
                 int debug_flag,
                 int random_flag)
				 
{
    DBG_PRINT(DBG_LEVEL_VDEBUG, "\n");
            
    g_PktMgr.addMsgEvent_Standard(new infoSeqNo());

    g_SetsMgr.setUserMode_ErrRate(error_rate);
    g_SetsMgr.setUserMode_ErrDrop(drop_flag);
    g_SetsMgr.setUserMode_ErrFlip(flip_flag);
    g_SetsMgr.setUserMode_SeedRand(random_flag ? time(NULL) : nextSeed);
	nextSeed =  (random_flag) ? time(NULL) : nextSeed + 1; // so any child forked will have a different seed value
    g_SetsMgr.setUserMode_Debug(debug_flag);

    char dflag = (drop_flag) ? 'Y' : 'N';
    char fflag = (flip_flag) ? 'Y' : 'N';
    char dbflag = (debug_flag) ? 'Y' : 'N';	
    char rflag = (random_flag) ? 'Y' : 'N';
	
    DBG_PRINT(DBG_LEVEL_INFO, "Send Err-INIT ErrRate: %.2f Drop: %c Flip: %c Rand: %c Debug: %c\n", error_rate, dflag, fflag, rflag, dbflag);
        
    return 0;
}
// ============================================================================
ssize_t sendErr  (int s, void *msg, int len, unsigned int flags)
{
    //DBG_PRINT(DBG_LEVEL_VDEBUG, "\n");

    return g_PktMgr.send_Err(s, msg, len, flags);
}
// ============================================================================
ssize_t recvErr(int s, void *buf, size_t len, int flags)
{
    //DBG_PRINT(DBG_LEVEL_VDEBUG, "\n");

    return g_PktMgr.recv_Mod(s, buf, len, flags);
}
// ============================================================================
ssize_t sendtoErr(int s, void *msg, int len, unsigned int flags,
              const struct sockaddr *to, int tolen)
{
    //DBG_PRINT(DBG_LEVEL_VDEBUG, "\n");

    return g_PktMgr.sendto_Err(s, msg, len, flags, to, tolen);
}
// ============================================================================
ssize_t recvfromErr(int s, void *buf, size_t len, int flags,
              struct sockaddr *from, socklen_t *fromlen)
{
    //DBG_PRINT(DBG_LEVEL_VDEBUG, "\n");

    return g_PktMgr.recvfrom_Mod(s, buf, len, flags, from, fromlen);
}
// ============================================================================
// ============================================================================
