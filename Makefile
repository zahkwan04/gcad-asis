# Makefile for SCAD Server.
#
# Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
#
# @version $Id: Makefile 1906 2025-02-25 02:28:38Z zulzaidi $
# @author Mohd Rashid
# @author Mohd Rozaimi

CC         = g++
DEBUG      = -g -DDEBUG
NDEBUG     = -DNDEBUG
BASIC      = -DBASIC -DNO_DB -DMOBILE_CLIENTS
LIBUCL_DIR = TRTP
INCLUDES   = -I. -ITMD5 -I$(LIBUCL_DIR)
CPPFLAGS   = -O2 -Wall -std=c++14 $(INCLUDES)

SVR_DIR = server
CLT_DIR = client
CMN_DIR = common
SERVER  = scads
CLIENT  = scadtc
#feature list - comment out unneeded ones
AES_DEF     = -DMSG_AES
AGW_DEF     = -DAGW
LICENSE_DEF = -DLICENSE
MOBILE_DEF  = -DMOBILE_CLIENTS
PEER_DEF    = -DPEER
SIPTCP_DEF  = -DSIPTCP
#SNMP_DEF    = -DSNMP
#RBTMQ_DEF   = -DRBTMQ
#STM integration
#STM_INT_DEF = -DSTM_INT

FEATURES_DEF = $(AES_DEF) $(AGW_DEF) $(LICENSE_DEF) $(MOBILE_DEF) $(PEER_DEF) \
               $(SNMP_DEF) $(RBTMQ_DEF) $(STM_INT_DEF)

OBJS_COMMON = TMD5/MD5.o \
              TMD5/Md5Digest.o \
              Utils.o \
              Logger.o \
              Socket.o \
              UdpSocket.o \
              TcpSocket.o \
              MsgSp.o \
              MsgSip.o \
              SubsData.o \
              StatusCodes.o \
              VoipSessionBase.o \
              DbInt.o

OBJS_SVR = $(OBJS_COMMON) \
           $(CMN_DIR)/Alert.o \
           $(CMN_DIR)/GpsLocation.o \
           $(CMN_DIR)/RedundancyManager.o \
           $(CMN_DIR)/main.o \
           $(SVR_DIR)/CfgManager.o \
           $(SVR_DIR)/MsgTdp.o \
           $(SVR_DIR)/TdpConverter.o \
           $(SVR_DIR)/TdpSession.o \
           $(SVR_DIR)/StmSession.o \
           $(SVR_DIR)/ClientSession.o \
           $(SVR_DIR)/VoipSession.o \
           $(SVR_DIR)/ClientManager.o \
           $(SVR_DIR)/CallSession.o \
           $(SVR_DIR)/FileMonitor.o \
           $(SVR_DIR)/Mms.o \
           $(SVR_DIR)/SdmAgentSession.o \
           $(SVR_DIR)/Controller.o \
           $(SVR_DIR)/ControllerInt.o

OBJS_SVR_EXT = $(CMN_DIR)/Language.o \
               $(CMN_DIR)/WebAuditTrailUi.o \
               $(CMN_DIR)/WebLogin.o \
               $(CMN_DIR)/WebServer.o \
               $(CMN_DIR)/WebStyle.o \
               $(SVR_DIR)/WebStatusMsgUi.o \
               $(SVR_DIR)/WebBranchUi.o \
               $(SVR_DIR)/WebSubsDataUi.o \
               $(SVR_DIR)/WebAdminUi.o \
               $(SVR_DIR)/WebUi.o

OBJS_CLT = $(OBJS_COMMON) \
           $(CLT_DIR)/VoipSessionClient.o \
           $(CLT_DIR)/RtpSession.o \
           $(CLT_DIR)/ServerSession.o \
           $(CLT_DIR)/testclient.o

OBJS_VOLATILE = VoipSessionBase.o SubsData.o

DEP_PAL = PalFiles.h \
          PalLock.h \
          PalSem.h \
          PalSocket.h \
          PalThread.h \
          PalTime.h

DEP_SVR_MAIN = $(CMN_DIR)/main.cpp $(SVR_DIR)/version.h

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
    INCLUDES += -I/usr/include/postgresql
    LIBS += -pthread -ldl -lwthttp -lwt -lssl -lcrypto
endif
LIBS += -lpq -L$(LIBUCL_DIR) -luclmm

#features
ifdef AES_DEF
    OBJS_COMMON += Aes.o
endif
ifdef AGW_DEF
    AGW_DIR      = $(SVR_DIR)/agw
    AGW_ACAPI    = $(AGW_DIR)/acapi
    OBJS_SVR_AGW = $(AGW_DIR)/AcapiSession.o \
                   $(AGW_DIR)/AcapiConverter.o \
                   $(AGW_DIR)/ServiceBase.o \
                   $(AGW_DIR)/ServiceMon.o \
                   $(AGW_DIR)/ServiceSds.o \
                   $(AGW_DIR)/ServiceSs.o \
                   $(AGW_DIR)/ServiceManager.o

    OBJS_SVR_EXT += $(OBJS_SVR_AGW)
    INCLUDES += -I$(AGW_DIR) -I$(AGW_ACAPI)
    LIBS += -L$(AGW_ACAPI) -lacapi
else
    #to prevent error in 'cleansvr'
    AGW_DIR = $(SVR_DIR)
endif
ifdef PEER_DEF
    OBJS_SVR += $(SVR_DIR)/PeerManager.o
endif
ifdef SNMP_DEF
    OBJS_SVR += $(SVR_DIR)/SnmpAgent.o
    LIBS += -lnetsnmp -lnetsnmpagent -lnetsnmphelpers
endif
ifdef RBTMQ_DEF
    INCLUDES += -Inlohmann
    OBJS_SVR += $(SVR_DIR)/RabbitMqInt.o
    LIBS += -lrabbitmq
endif
ifdef LICENSE_DEF
    OBJS_SVR += $(CMN_DIR)/LicManager.o
endif

all: debug

tdploopback: CPPFLAGS += -DTEST_TDP_LOOPBACK
tdploopback: debug

debug: CPPFLAGS += $(DEBUG)
debug: $(SERVER)

release: CPPFLAGS += $(NDEBUG) -DENABLE_DEBUG_LOG
release: $(SERVER)

basic: CPPFLAGS += $(DEBUG) $(BASIC)
basic: $(SERVER)basic

$(CLIENT): CPPFLAGS += $(DEBUG) -DTESTCLIENT $(AES_DEF) $(SIPTCP_DEF)
$(CLIENT): INCLUDES += -I$(CLT_DIR)
$(CLIENT): cleanvolatile $(OBJS_CLT)
	 $(CC) $(CPPFLAGS) -o $@ $(OBJS_CLT) $(LIBS)

$(SERVER): CPPFLAGS += -DSERVERAPP $(FEATURES_DEF)
$(SERVER): INCLUDES += -I$(SVR_DIR) -I$(CMN_DIR)
$(SERVER): cleanvolatile $(OBJS_SVR) $(OBJS_SVR_EXT)
	 $(CC) $(CPPFLAGS) -o $@ $(OBJS_SVR) $(OBJS_SVR_EXT) $(LIBS)

$(SERVER)basic: CPPFLAGS += -DSERVERAPP
$(SERVER)basic: INCLUDES += -I$(SVR_DIR) -I$(CMN_DIR)
$(SERVER)basic: cleanvolatile $(OBJS_SVR)
	 $(CC) $(CPPFLAGS) -o $(SERVER) $(OBJS_SVR) $(LIBS)

$(CMN_DIR)/main.o: $(DEP_SVR_MAIN)
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.o: %.cpp %.h $(DEP_PAL)
	$(CC) $(CPPFLAGS) -c -o $@ $<

clean: cleansvr cleantc
	-rm -f *.o

cleansvr:
	-rm -f $(SVR_DIR)/*.o $(CMN_DIR)/*.o $(AGW_DIR)/*.o $(SERVER)

cleansvrcmn:
	-rm -f $(CMN_DIR)/*.o

cleantc:
	-rm -f $(CLT_DIR)/*.o $(CLIENT)

cleanvolatile:
	-rm -f $(OBJS_VOLATILE)
