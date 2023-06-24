#include <algorithm>
#include "Node.h"
#include <cstdio> 

namespace WAMPL{
Define_Module(Node);

void Node::initialize()
{
    if(_access("OutputDeadDevice.txt",0) == 0) remove("OutputDeadDevice.txt");
    if(_access("OutputFatherID.txt",0) == 0) remove("OutputFatherID.txt");
    if(_access("OutputDead.txt",0) == 0) remove("OutputDead.txt");
    if(_access("OutputTopology.txt",0) == 0) remove("OutputTopology.txt");
    outPutDeadFlag = 0;
    deviceNum = par("deviceNum");
    maxTransDist = par("maxTransDist");
    x = par("x");
    y = par("y");
    loraType = par("loraType");
    RoutingInfoInit();
    if(loraType == 1){
        ThisDeviceInfo.EndDeviceBattery = maxBattery*20;
    }
    else ThisDeviceInfo.EndDeviceBattery = maxBattery + uniform(-maxBattery/20, maxBattery/20);
    initialMsg->setX(x);
    initialMsg->setY(y);
    lastAtionTime = round * cycleTime;
    initialMsg->setLastActionTime(lastAtionTime);
    scheduleAt(round, initialMsg);
    ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum = 0;
    ThisDeviceInfo.EndDeviceRank = 0;
    ThisDeviceInfo.EndDevicePacketLength = controlPacketLength + dataPacketLength;
    ifDead = false;
    otherNode = getParentModule()->getSubmodule("node", this->getId() - 2);
    otherNode->par("fatherID") = this->getId() - 2;
    otherNode->par("battery") = ThisDeviceInfo.EndDeviceBattery;
}

void Node::handleMessage(cMessage *msg)
{
    wmsg *inMsg = check_and_cast<wmsg *>(msg);
    if(ThisDeviceInfo.EndDeviceBattery <= 0){// && !inMsg->isSelfMessage()){
        std::string str= "Dead";
        getDisplayString().setTagArg("t", 0, str.c_str());
        ifDead = true;
        OutputDead();
        networkedFlag = 0;
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("ifDead") = ifDead;
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("fatherID") = -2;
        return;
    }
    if(!inMsg->isSelfMessage())
    {
        EV<<round<<',';
        EV<<inMsg->getSourceID()<<"->"<<ThisDeviceInfo.EndDeviceID<<'\n';
        //EV<<inMsg->getX()<<','<<x<<'\n';
    }
    
    double distance = sqrt((inMsg->getX() - x)*(inMsg->getX() - x) + (inMsg->getY() - y)*(inMsg->getY() - y));
    EV<<distance<<','<<maxTransDist<<'\n';
    if(distance > maxTransDist && !inMsg->isSelfMessage()){
        EV<<"too far"<<'\n';
        return;
    }
    //wireless signal
    if(strcmp("networkingMsg", inMsg->getFullName()) == 0 && networkedFlag == 0){
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
        int neiborDevicePosition = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceID = inMsg->getSourceID();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceRank = inMsg->getSourceRank();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDevicePriority = inMsg->getSourcePriority();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceBattery = inMsg->getSourceBattery();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceDist = distance;
        //ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum].FatherEndDeviceSNR = rxsnr;
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum++;
        //ThisDeviceRoutingInfo.IfDeviceReceived[sourceDeviceID] = 1;
        EV <<"networking received! Father id is "<<inMsg->getSourceID();
        if(networkedFlag == 0){
            lastAtionTime = inMsg->getLastActionTime() + interval;
            PreparePacket(calcFatherMsg, 0, lastAtionTime, 0);
            rescheduleAt(lastAtionTime, calcFatherMsg);
        }
    }

    if(strcmp("uploadingMsg", inMsg->getFullName()) == 0){
        int sourcePacketLength = inMsg->getSourcePacketLength();
        ThisDeviceInfo.EndDevicePacketLength = sourcePacketLength + dataPacketLength;
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * (sourcePacketLength + 30);
        lastAtionTime = inMsg->getLastActionTime() + interval;
        otherNode = getParentModule()->getSubmodule("node", inMsg->getSourceID());
        wmsg *msgcoppy = uploadedMsg->dup();
        PreparePacket(msgcoppy, inMsg->getSourceID(), lastAtionTime, controlPacketLength);
        //sendDirect(msgcoppy, interval, 0, otherNode, "port");
        if(!ifDead) sendDirect(msgcoppy, otherNode, "port");
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
    }

    if(strcmp("uploadedMsg", inMsg->getFullName()) == 0){
        if(inMsg->getSourceBattery() < 0 || inMsg->getSourceNetworkedFlag() == 0){
            networkedFlag = 0;
            return;
        }
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;

        lastAtionTime = inMsg->getLastActionTime() + singleTransmitCycle;
        PreparePacket(normalTransmit, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, normalTransmit);
    }

    //self msg
    if(strcmp("initialMsg", inMsg->getFullName()) == 0){
        
        //add sunpower
        if(1){
            ThisDeviceInfo.EndDeviceBattery+=chargeRate;
            if(ThisDeviceInfo.EndDeviceBattery>maxBattery && loraType != 1 ) ThisDeviceInfo.EndDeviceBattery = maxBattery;

            if(ThisDeviceInfo.EndDeviceID == 7 && round == 10){
                ThisDeviceInfo.EndDeviceBattery = 1.2;
                chargeRate = 0.2;
            }
            if(ThisDeviceInfo.EndDeviceID == 8 && round == 10){
                ThisDeviceInfo.EndDeviceBattery = 1;
                chargeRate = 0.2;
            }
        }
        OutputBattery();
        EndDeviceInit();
        RoutingInfoInit();

        //OutputDead();
        if(networkedFlag == 0){
            std::string str= "Not networked";
            getDisplayString().setTagArg("t", 0, str.c_str());
        }
        if(loraType == 1)
        {
            std::string str= "Gateway";
            getDisplayString().setTagArg("t", 0, str.c_str());
            networkedFlag = 1;
            lastAtionTime = inMsg->getLastActionTime() + interval;
            PreparePacket(broadcastNetworkingMsg, 0, lastAtionTime, 0);
            scheduleAt(lastAtionTime, broadcastNetworkingMsg);
        }
        else{
            lastAtionTime = round*cycleTime + networkingTime;
            wmsg *msgcoppy = normalTransmit->dup();
            PreparePacket(msgcoppy, 0, lastAtionTime, 0);
            scheduleAt(lastAtionTime, msgcoppy);
        }
        lastAtionTime = (round+1)*cycleTime;
        PreparePacket(resetNetork, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, resetNetork);
    }

    if(strcmp("broadcastNetworkingMsg", inMsg->getFullName()) == 0){
        lastAtionTime = inMsg->getLastActionTime() + interval;
        PreparePacket(networkingMsg, 0, lastAtionTime, controlPacketLength);
        Broadcast(networkingMsg);
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
    }

    if(strcmp("calcFatherMsg", inMsg->getFullName()) == 0){
        SelectFatherEndDevice();
        if(networkedFlag == 1){
            std::string str= "FatherID: ";
            str += std::to_string(ThisDeviceRoutingInfo.FatherEndDeviceID);
            getDisplayString().setTagArg("t", 0, str.c_str());
            lastAtionTime = inMsg->getLastActionTime() + interval;
            PreparePacket(broadcastNetworkingMsg, 0, lastAtionTime, 0);
            scheduleAt(lastAtionTime, broadcastNetworkingMsg);
        }
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("fatherID") = ThisDeviceRoutingInfo.FatherEndDeviceID;
    }

    if(strcmp("normalTransmit", inMsg->getFullName()) == 0){
        otherNode = getParentModule()->getSubmodule("node", ThisDeviceRoutingInfo.FatherEndDeviceID);
        if(otherNode ==nullptr) return;
//        lastAtionTime = inMsg->getLastActionTime() + singleTransmitCycle;
//        PreparePacket(normalTransmit, 0, lastAtionTime);
//        scheduleAt(lastAtionTime, normalTransmit);
        lastAtionTime = inMsg->getLastActionTime() + interval;
        EV<<"father id:"<<ThisDeviceRoutingInfo.FatherEndDeviceID;
        wmsg *msgcoppy = uploadingMsg->dup();
        msgcoppy->setSourceID(ThisDeviceInfo.EndDeviceID);
        PreparePacket(msgcoppy, ThisDeviceRoutingInfo.FatherEndDeviceID, lastAtionTime, ThisDeviceInfo.EndDevicePacketLength);
        //sendDirect(msgcoppy, interval, 0, otherNode, "port");
        if(!ifDead) sendDirect(msgcoppy, otherNode, "port");
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * ThisDeviceInfo.EndDevicePacketLength;
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("battery") = ThisDeviceInfo.EndDeviceBattery;
    }

    if(strcmp("resetNetork", inMsg->getFullName()) == 0){
        OutputTopology();
        round++;
        lastAtionTime = inMsg->getLastActionTime() + interval;
        PreparePacket(initialMsg, 0, lastAtionTime, 0);
        cancelEvent(normalTransmit);
        scheduleAt(lastAtionTime, initialMsg);
        OutputFatherID();
        networkedFlag = 0;
    }
}

void Node::EndDeviceInit()
{
    otherNode = getParentModule()->getSubmodule("node", this->getId() - 2);
    otherNode->par("fatherID") = -2;
    ThisDeviceInfo.EndDeviceID = this->getId() - 2;
    ThisDeviceInfo.EndDeviceRank = 0;
    ThisDeviceInfo.EndDevicePriority = 0;
    //ThisDeviceInfo.EndDeviceBattery = maxBattery;
    ThisDeviceInfo.EndDevicePacketLength = dataPacketLength + controlPacketLength;
}

void Node::RoutingInfoInit(){
    ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum = 0;
    ThisDeviceRoutingInfo.FatherEndDeviceID = -2;
    ThisDeviceRoutingInfo.SonEndDeviceNum = 0;
}

void Node::Broadcast(wmsg *txpacket)
{
    if(ifDead == false){
        for(int i = 0; i < deviceNum; ++i){
            if(i == ThisDeviceInfo.EndDeviceID) continue;
            //EV<<"id:"<<this->getId()<<','<<i<<'\n';
            otherNode = getParentModule()->getSubmodule("node", i);
            wmsg *msgcoppy = txpacket->dup();
            if(!ifDead) sendDirect(msgcoppy, otherNode, "port");//otherNode->gate("port"));
        }
    }
}

void Node::SelectFatherEndDevice(){
    int neiborNum = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
    if(neiborNum == 0) return;
    double weight[neiborNum];
    for(int i = 0; i< neiborNum; ++i){
        weight[i] = (50*ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceBattery/maxBattery);// + 50*(maxTransDist - ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceDist)/maxTransDist);
    }
    int maxWeight = weight[0];
    int pos = 0;
    for(int i = 1; i< neiborNum; ++i){
        if(weight[i] > maxWeight){
            maxWeight = weight[i];
            pos = i;
        } 
    }
    ThisDeviceRoutingInfo.FatherEndDeviceID = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceID;
    ThisDeviceInfo.EndDeviceRank = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceRank + 1;
    networkedFlag = 1;
    EV <<"networked! Father id is "<<ThisDeviceRoutingInfo.FatherEndDeviceID<<','<<simTime()<<','<< ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceDist;
}

void Node::PreparePacket(wmsg *txpacket, int destID, double lastActionTime, int packetLength){
    txpacket->setDestID(destID);
    txpacket->setSourceID(ThisDeviceInfo.EndDeviceID);
    txpacket->setLastActionTime(lastActionTime);
    txpacket->setX(x);
    txpacket->setY(y);
    txpacket->setSourceBattery(ThisDeviceInfo.EndDeviceBattery);
    txpacket->setSourceNetworkedFlag(networkedFlag);
    txpacket->setSourcePacketLength(packetLength);
}

void Node::OutputBattery(){
    lastAtionTime = simTime().dbl() + interval*10;
    // PreparePacket(selfCheck, 0, lastAtionTime, 0);
    // rescheduleAt(lastAtionTime, selfCheck);
    const char* filenameOutputBattery = "OutputDeadDevice.txt";
    FILE *fileOutputBattery;
    //bool ifDead = getParentModule()->getSubmodule("node", i)->par("ifDead");
    fileOutputBattery = fopen(filenameOutputBattery, "a+");
    double nowTime = simTime().dbl();
    fprintf(fileOutputBattery, "%d,%f,%f,%d,%d\n", ThisDeviceInfo.EndDeviceID, nowTime, ThisDeviceInfo.EndDeviceBattery, round, ThisDeviceRoutingInfo.FatherEndDeviceID);
    fclose(fileOutputBattery);
}

int Node::findNeiborDevicePosition(int neiborDeviceID){
    int neiborNum = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
    for(int i = 0; i < neiborNum; i++){
        if(ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceID == neiborDeviceID){
            return i;
        }
    }
    return ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
}

void Node::OutputFatherID(){
    const char* filenameOutputFatherID = "OutputFatherID.txt";
    FILE *fileOutputFatherID;
    fileOutputFatherID = fopen(filenameOutputFatherID, "a+");
    fprintf(fileOutputFatherID, "%f,%d,%d\n", simTime().dbl(), ThisDeviceInfo.EndDeviceID, ThisDeviceRoutingInfo.FatherEndDeviceID);
    fclose(fileOutputFatherID);
}
void Node::OutputDead(){
    const char* filenameOutputDead = "OutputDead.txt";
    FILE *fileOutputDead;
    fileOutputDead = fopen(filenameOutputDead, "a+");
    if(ifDead == true && outPutDeadFlag == 0){
        outPutDeadFlag = 1;
        EV << "OUTPUTDEAD"<<'\n';
        fprintf(fileOutputDead, "%d, %f\n", ThisDeviceInfo.EndDeviceID, simTime().dbl());
    }
    for(int i = 2; i < 14; ++i){
        double b = getParentModule()->getSubmodule("node", i)->par("battery");
        fprintf(fileOutputDead,"%.2f,", b);
    }
    fprintf(fileOutputDead,"\n");
    fclose(fileOutputDead);
}

void Node::OutputTopology(){
    const char* filenameOutput = "OutputTopology.txt";
    FILE *fileOutput;
    fileOutput = fopen(filenameOutput, "a+");
    for(int i = 2; i < 14; ++i){
        fprintf(fileOutput,"%d,", (int)(getParentModule()->getSubmodule("node", i)->par("fatherID")));
    }
    for(int i = 2; i < 14; ++i){
        double b = getParentModule()->getSubmodule("node", i)->par("battery");
        fprintf(fileOutput,"%.2f,", b);
    }
    fprintf(fileOutput,"%d,", round);
    fprintf(fileOutput,"\n");
    fclose(fileOutput);
}
}
