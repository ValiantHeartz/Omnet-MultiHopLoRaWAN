
#include <omnetpp.h>
#include <string>
#include "wmsg_m.h"

using namespace omnetpp;

namespace WAMPL {
typedef struct sEndDeviceInfo{
  int EndDeviceID;
  int EndDeviceRank;
  int EndDevicePriority;
  double EndDeviceBattery;
  int EndDevicePacketLength;
  //Channel parameters;
}EndDeviceInfo_t;

typedef struct sFatherEndDevice{
  int FatherEndDeviceID;
  int FatherEndDevicePriority;
  double FatherEndDeviceBattery;
  int FatherEndDeviceRank;
  double FatherEndDeviceDist;
  int FatherEndDeviceSNR;
}FatherEndDevice_t;

typedef struct sEndDeviceRoutingInfo_t{
  int NeiborFatherEndDeviceNum;
  int FatherEndDeviceID;
  FatherEndDevice_t NeiborFatherEndDeviceInfo[10];
  int SonEndDeviceNum;
  int SonEndDeviceID[10];
  int SonEndDeviceIDAssigned[20];
  int IfDeviceReceived[20];
}EndDeviceRoutingInfo_t;

class Node : public cSimpleModule
{
  private:
    //node para
    int loraType;
    int deviceNum;
    int networkedFlag = 0;
    double x, y;
    int round = 1;
    double lastAtionTime = 0;
    const double interval = 0.1;
    const int cycleTime = 600; //cycle length
    const double networkingTime = 10*interval;
    const double singleTransmitCycle = 1;
    const double txrxEnergy = 50E-7/2;//E-9;
    const int controlPacketLength = 13;
    const int dataPacketLength = 22;
    const double maxBattery = 3.33; //5mah
    int maxTransDist;
    bool ifDead;
    double chargeRate = 0.5;
    
    wmsg *networkingMsg = new wmsg("networkingMsg");
    wmsg *networkedMsg = new wmsg("networkedMsg");
    wmsg *assignPriorityMsg = new wmsg("assignPriorityMsg");
    wmsg *uploadingMsg = new wmsg("uploadingMsg");
    wmsg *uploadedMsg = new wmsg("uploadedMsg");

    //self msg
    wmsg *initialMsg = new wmsg("initialMsg");
    wmsg *broadcastNetworkingMsg = new wmsg("broadcastNetworkingMsg");
    wmsg *calcFatherMsg = new wmsg("calcFatherMsg");
    wmsg *normalTransmit = new wmsg("normalTransmit");
    wmsg *resetNetork = new wmsg("resetNetork");
    wmsg *selfCheck = new wmsg("selfCheck");

    EndDeviceInfo_t ThisDeviceInfo;
    EndDeviceRoutingInfo_t ThisDeviceRoutingInfo;

    //omnetpp para
    cModule *otherNode;

    //fileoutput
    int outPutDeadFlag;
    int donothingflag = 0;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void EndDeviceInit();
    virtual void RoutingInfoInit();
    virtual void Broadcast(wmsg *txpacket);
    virtual void PreparePacket(wmsg *txpacket, int destID, double lastActionTime, int packetLength);
    virtual void SelectFatherEndDevice();
    virtual void OutputBattery();
    virtual void OutputFatherID();
    virtual int findNeiborDevicePosition(int neiborDeviceID);
    virtual void OutputDead();
    virtual void OutputTopology();
};
}
