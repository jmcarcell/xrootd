#ifndef __OLB_MANAGER__H
#define __OLB_MANAGER__H
/******************************************************************************/
/*                                                                            */
/*                      X r d O l b M a n a g e r . h h                       */
/*                                                                            */
/* (c) 2003 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

//         $Id$

#include <stdlib.h>
#include <string.h>
#include <strings.h>
  
#include "XrdOlb/XrdOlbManList.hh"
#include "XrdOlb/XrdOlbTypes.hh"
#include "XrdOuc/XrdOucPthread.hh"

class XrdNetLink;
class XrdOlbDrop;
class XrdOlbServer;

// Options for ListServers
//
#define OLB_LS_BEST   0x0001
#define OLB_LS_ALL    0x0002

// Status flags
//
#define OLB_SERVER_DISABLE 0x0001
#define OLB_SERVER_NOSTAGE 0x0002
#define OLB_SERVER_OFFLINE 0x0004
#define OLB_SERVER_SUSPEND 0x0008
#define OLB_SERVER_NOSPACE 0x0020

/******************************************************************************/
/*                            o o l b _ S I n f o                             */
/******************************************************************************/
  
class XrdOlbSInfo
{
public:

XrdOlbSInfo *next;
char        *Name;
int          Id;
int          Load;
int          Free;
int          RefTotA;
int          RefTotR;
int          Status;

            XrdOlbSInfo(const char *sname, XrdOlbSInfo *np=0)
                      {Name = (sname ? strdup(sname) : 0); next=np;}

           ~XrdOlbSInfo() {if (Name) free(Name);}
};
 
/******************************************************************************/
/*                          S t a t u s   F l a g s                           */
/******************************************************************************/

// Flags passed to the AddServer()
//
const int OLB_noStage =  1;
const int OLB_Suspend =  2;
const int OLB_Special =  4;
const int OLB_isMan   =  8;
const int OLB_Lost    = 16;
  
/******************************************************************************/
/*                          o o l b _ M a n a g e r                           */
/******************************************************************************/
  
// This a single-instance global class
//
class XrdOlbManager
{
public:
friend class XrdOlbDrop;

static const int MTMax = 16;   // Maximum number of Managers
static const int STMax = 64;   // Maximum number of Subscribers

int         ServCnt;           // Number of active subscribers
char        noData;            // Set when we are a manager or supervisor

void        Broadcast(SMask_t smask, char *buff, int blen);
void        Broadcast(SMask_t smask, const struct iovec *, int iovcnt);
int         haveManagers() {return MTHi >= 0;};
void        Inform(const char *cmd, int clen=0, char *arg=0, int alen=0);
void        Inform(SMask_t mmask, const char *cmd, int clen=0);
XrdOlbSInfo *ListServers(SMask_t mask=(SMask_t)-1, int opts=0);
void       *Login(XrdNetLink *lnkp);
void       *MonPerf(void);
void       *MonPing(void);
void       *MonRefs(void);
void       *Pander(char *manager, int port);
void        Remove_Server(const char *reason, int sent, int sinst, int immed=0);
void        Reset(void);
void        ResetRef(SMask_t smask);
void        Resume();
int         SelServer(int pt, char *path, SMask_t pmsk, SMask_t amsk, char *hb,
                      const struct iovec *iodata=0, int iovcnt=0);
void        setPort(int port) {Port = port;}
void        Snooze(int slpsec);
void        Space(int none, int doinform=1);
void        Stage(int ison, int doinform=1);
int         Stats(char *bfr, int bln);
void        Suspend(int doinform=1);

      XrdOlbManager();
     ~XrdOlbManager() {} // This object should never be deleted

private:
SMask_t       AddPath(XrdOlbServer *sp);
int           Add_Manager(XrdOlbServer *sp);
XrdOlbServer *AddServer(XrdNetLink *lp, int port, int Status, 
                                        int sport, char *sid);
XrdOlbServer *calcDelay(int nump, int numd, int numf, int numo,
                        int nums, int &delay, char **reason);
int           Drop_Server(int sent, int sinst, XrdOlbDrop *djp=0);
void         *Login_Failed(const char *reason, XrdNetLink *lp, XrdOlbServer *sp=0);
void          Record(char *path, const char *reason);
void          Remove_Manager(const char *reason, XrdOlbServer *sp);
XrdOlbServer *SelbyLoad(SMask_t mask, int &nump, int &delay,
                        char **reason, int needspace);
XrdOlbServer *SelbyRef( SMask_t mask, int &nump, int &delay,
                        char **reason, int needspace);
void          sendAList(XrdNetLink *lp);
void          setAltMan(int snum, unsigned int ipaddr, int port);

static const  int AltSize = 24;

XrdOucMutex   XXMutex;
XrdOucMutex   STMutex;
XrdOlbServer *ServTab[STMax];
XrdOlbServer *ServBat[STMax];
XrdOucMutex   MTMutex;
XrdOlbServer *MastTab[MTMax];
char          AltMans[STMax*AltSize]; // ||123.123.123.123:12345|| = 21
char         *AltMend;
int           AltMent;

int  MTHi;
int  STHi;
int  InstNum;
int  XWait;
int  XnoStage;
int  Port;
int  SelAcnt;
int  SelRcnt;
int  doReset;
SMask_t resetMask;
};
#endif
