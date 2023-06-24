#include <algorithm>
#include "Node.h"
#include <cstdio> 
using namespace std;
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

    ifDead = false;
    ThisDeviceInfo.EndDeviceRank = 0;
    ThisDeviceInfo.EndDevicePacketLength = controlPacketLength + dataPacketLength;
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
        EV<<"ThisDeviceInfo.EndDeviceBattery <= 0";
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("fatherID") = -2;
        return;
    }
    if(!inMsg->isSelfMessage())
    {
        EV<<inMsg->getSourceID()<<"->"<<ThisDeviceInfo.EndDeviceID<<'\n';
        distance = sqrt((inMsg->getX() - x)*(inMsg->getX() - x) + (inMsg->getY() - y)*(inMsg->getY() - y));
        EV<<"DISTANCE is: "<<distance<<endl;
        if(distance > maxTransDist){
            EV<<"Signal overed!";
            return;
        }
    }
    
    //wireless signal
    if(strcmp("networkingMsg", inMsg->getFullName()) == 0 && networkedFlag == 0){
        int sourceId = inMsg->getSourceID();
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
        int neiborDevicePosition = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
        neiborIdMap[sourceId] = neiborDevicePosition;
        
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceID = sourceId;
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceRank = inMsg->getSourceRank();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDevicePriority = inMsg->getSourcePriority();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceBattery = inMsg->getSourceBattery();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceDist = distance;
        //ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum].FatherEndDeviceSNR = rxsnr;
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum++;
        //ThisDeviceRoutingInfo.IfDeviceReceived[sourceDeviceID] = 1;
        //EV <<"networking received! Father id is "<<inMsg->getSourceID();
        if(networkedFlag == 0){
            lastAtionTime = inMsg->getLastActionTime() + interval;
            PreparePacket(calcFatherMsg, 0, lastAtionTime, 0);
            rescheduleAt(lastAtionTime, calcFatherMsg);
        }
    }

    if(strcmp("uploadingMsg", inMsg->getFullName()) == 0){
        int sourceId = inMsg->getSourceID();
        if(!neiborIdMap.count(sourceId)){
            int neiborDevicePosition = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceID = sourceId;
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceRank = inMsg->getSourceRank();
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDevicePriority = inMsg->getSourcePriority();
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceBattery = inMsg->getSourceBattery();
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborDevicePosition].FatherEndDeviceDist = distance;
            neiborIdMap[sourceId] = neiborDevicePosition;
            sonborIdMap[sourceId] = neiborDevicePosition;
            ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum++;
        }
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborIdMap[sourceId]].FatherEndDeviceBattery = inMsg->getSourceBattery();
        
        //negotiate
        //EV<<"inMsg->getId(): "<<inMsg->getId()<<"inMsg->getNetworkingToSonFlagSelectedId(): "<<inMsg->getNetworkingToSonFlagSelectedId()<<endl;
        if(networkingToSonId != -2 && (int)inMsg->getSourceID() == networkingToSonId && (int)inMsg->getNetworkingToSonFlagSelectedId() != ThisDeviceInfo.EndDeviceID){
            waitNegotiateTime--;
            EV<<"waitNegotiateTime: "<<waitNegotiateTime<<endl;
            if(waitNegotiateTime == 0){
                networkingToSonId = -2;
            }
        }
        EV<<"NetworkingToSonId: "<<networkingToSonId<<endl;
        // if(inMsg->getNetworkingToSonFlagMsg() && networkedByFatherId == -2){
        //     networkedByFatherId = sourceId;
        // }
        int sourcePacketLength = inMsg->getSourcePacketLength();
        ThisDeviceInfo.EndDevicePacketLength = sourcePacketLength + dataPacketLength;
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * (sourcePacketLength + 30);  //30 for waiting signal 
        lastAtionTime = inMsg->getLastActionTime() + interval;
        otherNode = getParentModule()->getSubmodule("node", inMsg->getSourceID());
        wmsg *msgcopy = uploadedMsg->dup();
        PreparePacket(msgcopy, inMsg->getSourceID(), lastAtionTime, controlPacketLength);
        msgcopy->setBackwardScore(deviceBackwardScore);
        //sendDirect(msgcopy, interval, 0, otherNode, "port");
        if(!ifDead) sendDirect(msgcopy, otherNode, "port");
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
        
    }

    if(strcmp("uploadedMsg", inMsg->getFullName()) == 0){
        if(inMsg->getSourceBattery() < 0 || inMsg->getSourceNetworkedFlag() == 0){
            networkedFlag = 0;
            return;
        }
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
        int sourceId = inMsg->getSourceID();
        ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborIdMap[sourceId]].FatherEndDeviceBattery = inMsg->getSourceBattery();

        //negotiate
        // if(inMsg->getNetworkingToSonFlagMsg() == ThisDeviceInfo.EndDeviceID && networkedByFatherId == -2){
        //     canBeBackwardAndBackwardToFatherID = -2;
        //     for(auto i = neiborIdMap.begin(); i != neiborIdMap.end(); ++i){
        //         EV<<"neiborID/rank: "<<i->first<<','<<ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i->second].FatherEndDeviceRank<<','<<endl;
        //         if(i->first != sourceId && ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i->second].FatherEndDeviceRank < ThisDeviceInfo.EndDeviceRank){
        //             canBeBackwardAndBackwardToFatherID = i->first;
        //         }
        //     }
        //     if(inMsg->getSourceBattery() > ThisDeviceInfo.EndDeviceBattery) canBeBackwardAndBackwardToFatherID = -2;
        //     if(canBeBackwardAndBackwardToFatherID != -2){
        //         networkedByFatherId = sourceId;
        //         EV<<"NetworkingToSonId changed to -2!"<<endl;
        //         networkingToSonId = -2;
        //     } 
        // }
        //negotiate2
        if(inMsg->getNetworkingToSonFlagMsg() == ThisDeviceInfo.EndDeviceID){
            networkedByFatherId = sourceId;
        }
        if(inMsg->getNetworkingToSonFlagMsg() != -2) deviceBackwardScore = inMsg->getBackwardScore();
        EV<<"Networked by father: "<<networkedByFatherId<<endl;

        lastAtionTime = inMsg->getLastActionTime() + singleTransmitCycle;
        PreparePacket(normalTransmit, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, normalTransmit);
    }

    if(strcmp("broadcastJudgeIfBackwardValid", inMsg->getFullName()) == 0){
        EV<<"sourcescore/thisscore: "<<inMsg->getBackwardScore()<<','<<deviceBackwardScore<<endl;
        if(inMsg->getBackwardScore() < deviceBackwardScore){
            networkedByFatherId = inMsg->getSourceID();
            lastAtionTime = inMsg->getLastActionTime() + interval;
            otherNode = getParentModule()->getSubmodule("node", inMsg->getSourceID());
            wmsg *msgcopy = broadcastJudgeIfBackwardValidAck->dup();
            PreparePacket(msgcopy, 0, lastAtionTime, controlPacketLength);
            msgcopy->setBackwardScore(deviceBackwardScore);
            if(!ifDead) sendDirect(msgcopy, otherNode, "port");
            EV<<"Valided!"<<endl;
        }
    }
    if(strcmp("broadcastJudgeIfBackwardValidAck", inMsg->getFullName()) == 0){
        // if(ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[neiborIdMap[inMsg->getSourceID()]].FatherEndDeviceRank > ThisDeviceInfo.EndDeviceRank){
        //     if(inMsg->getNetworkingToSonFlagMsg() != -2) networkedByFatherId = -2;
        //     lastAtionTime = inMsg->getLastActionTime() + interval;
        //     otherNode = getParentModule()->getSubmodule("node", ThisDeviceRoutingInfo.FatherEndDeviceID);
        //     wmsg *msgcopy = broadcastJudgeIfBackwardValidAck->dup();
        //     PreparePacket(msgcopy, 0, lastAtionTime, controlPacketLength);
        //     if(!ifDead) sendDirect(msgcopy, otherNode, "port");
        // }
        // else{
        //     if(inMsg->getNetworkingToSonFlagSelectedId() != ThisDeviceInfo.EndDeviceID) networkingToSonId = -2;
        // }
        if(inMsg->getBackwardScore() > deviceBackwardScore && networkedByFatherId != -2){
            deviceBackwardScore = inMsg->getBackwardScore();
            networkingToSonId = inMsg->getSourceID();
            lastAtionTime = inMsg->getLastActionTime() + interval;
            otherNode = getParentModule()->getSubmodule("node", ThisDeviceRoutingInfo.FatherEndDeviceID);
            wmsg *msgcopy = broadcastJudgeIfBackwardValidAck->dup();
            PreparePacket(msgcopy, 0, lastAtionTime, controlPacketLength);
            if(!ifDead) sendDirect(msgcopy, otherNode, "port");
            EV<<"Son send to father to verify backward!"<<endl;
        }
        else{
            networkingToSonId = inMsg->getSourceID();
            EV<<"father backward succeess!"<<endl;
        }
    }

    //self msg
    if(strcmp("initialMsg", inMsg->getFullName()) == 0){

        //add sunpower
        if(1){
            ThisDeviceInfo.EndDeviceBattery+=chargeRate;
            if(ThisDeviceInfo.EndDeviceBattery>maxBattery && loraType != 1) ThisDeviceInfo.EndDeviceBattery = maxBattery;
            if(ThisDeviceInfo.EndDeviceID == 7 && round == 10){
                ThisDeviceInfo.EndDeviceBattery = 1;
                chargeRate = 0.2;
            }
            // if(ThisDeviceInfo.EndDeviceID == 4 && round == 10){
            //     ThisDeviceInfo.EndDeviceBattery = 1;
            //     chargeRate = 0.2;
            // }
            if(ThisDeviceInfo.EndDeviceID == 8 && round == 10){
                ThisDeviceInfo.EndDeviceBattery = 1.2;
                chargeRate = 0.2;
            }
            // if(ThisDeviceInfo.EndDeviceID == 3 && round == 10){
            //     ThisDeviceInfo.EndDeviceBattery = 1;
            //     chargeRate = 0.2;
            // }
        }

        if(donothingflag == 0){
            EndDeviceInit();
            RoutingInfoInit();
            neiborIdMap.clear();
            sonborIdMap.clear();
            //networkingToSonId = -2;
            networkedByFatherId = -2;
            waitNegotiateTime = 3;
        }
        OutputBattery();
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
            if(donothingflag == 0) scheduleAt(lastAtionTime, broadcastNetworkingMsg);
        }
        else{
            lastAtionTime = round*cycleTime + networkingTime;
            wmsg *msgcopy = normalTransmit->dup();
            PreparePacket(msgcopy, 0, lastAtionTime, 0);
            scheduleAt(lastAtionTime, msgcopy);
        }
        lastAtionTime = (round+1)*cycleTime;
        PreparePacket(resetNetork, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, resetNetork);

        lastAtionTime = (round+1)*cycleTime - singleTransmitCycle*10;
        PreparePacket(judgeIfNetworkingToSon, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, judgeIfNetworkingToSon);

        lastAtionTime = (round+1)*cycleTime - singleTransmitCycle*0.5;
        PreparePacket(selfJudgeIfBackwardValid, 0, lastAtionTime, 0);
        scheduleAt(lastAtionTime, selfJudgeIfBackwardValid);
        
    }

    if(strcmp("broadcastNetworkingMsg", inMsg->getFullName()) == 0){
        lastAtionTime = inMsg->getLastActionTime() + interval;
        PreparePacket(networkingMsg, 0, lastAtionTime, controlPacketLength);
        Broadcast(networkingMsg);
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * controlPacketLength;
    }

    if(strcmp("calcFatherMsg", inMsg->getFullName()) == 0){
        EV<<"networkingToSonId: "<<networkingToSonId<<endl;
        if(networkingToSonId == -2 || neiborIdMap.count(networkingToSonId)){
            int pos;
            if(networkingToSonId == -2) pos = SelectFatherEndDevice();
            else pos = neiborIdMap[networkingToSonId];
            ThisDeviceRoutingInfo.FatherEndDeviceID = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceID;
            if(round == 1) ThisDeviceInfo.EndDeviceRank = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceRank + 1;
            networkedFlag = 1;
            networkingToSonId = -2;
            EV <<"networked! Father id "<<ThisDeviceRoutingInfo.FatherEndDeviceID<<endl;
        }
        else{
            return;
        }
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
        lastAtionTime = inMsg->getLastActionTime() + interval;
        EV<<"father id:"<<ThisDeviceRoutingInfo.FatherEndDeviceID;
        wmsg *msgcopy = uploadingMsg->dup();
        msgcopy->setSourceID(ThisDeviceInfo.EndDeviceID);
        PreparePacket(msgcopy, ThisDeviceRoutingInfo.FatherEndDeviceID, lastAtionTime, ThisDeviceInfo.EndDevicePacketLength);
        //sendDirect(msgcopy, interval, 0, otherNode, "port");
        if(!ifDead) sendDirect(msgcopy, otherNode, "port");
        ThisDeviceInfo.EndDeviceBattery -= txrxEnergy * ThisDeviceInfo.EndDevicePacketLength;
        getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("battery") = ThisDeviceInfo.EndDeviceBattery;
    }

    if(strcmp("resetNetork", inMsg->getFullName()) == 0){
        // EV<<"Network status: "<<endl;
        // for(int i = 0; i < 14; ++i){
        //     EV<<"father:"<<(int)getParentModule()->getSubmodule("node", i)->par("fatherID")<<',';
        // }
        // EV<<endl;
        EV<<"ID/battttery: "<<ThisDeviceInfo.EndDeviceID<<", "<<ThisDeviceInfo.EndDeviceBattery<<" round: "<<round<<" networkingToSonId: "<<networkingToSonId<<" canBeBackwardAndBackwardToFatherID: "<<canBeBackwardAndBackwardToFatherID<<endl;
        OutputTopology();

        round++;
        lastAtionTime = inMsg->getLastActionTime() + interval;
        PreparePacket(initialMsg, 0, lastAtionTime, 0);
        cancelEvent(normalTransmit);
        scheduleAt(lastAtionTime, initialMsg);
        OutputFatherID();
        //donothingflag = 1; //????????
        networkedFlag = 0; //????????
       
    }

    if(strcmp("judgeIfNetworkingToSon", inMsg->getFullName()) == 0){

        int pos = SelectFatherEndDevice();
        int sonId = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceID;
        int sonRank = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceRank;
        double sonBat = ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[pos].FatherEndDeviceBattery;
        if(networkingToSonId == -2){
            if(sonborIdMap.count(sonId) && sonRank > ThisDeviceInfo.EndDeviceRank && sonBat - ThisDeviceInfo.EndDeviceBattery > 0) networkingToSonId = sonId;
        }
        else{
            if(sonId != networkingToSonId) networkingToSonId = -2;
        }
        deviceBackwardScore = ThisDeviceInfo.EndDeviceBattery;
        EV<<"round: "<<round<<endl;
        EV<<"want to "<<sonId<<" judgeIfNetworkingToSon:"<<networkingToSonId<<" All to be selected: "<<endl;
        EV<<"bat: son/father: "<<sonBat<<','<<ThisDeviceInfo.EndDeviceBattery<<endl;
        for(int i = 0; i < ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum; ++i){
            EV<<ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceID<<','<<ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceBattery<<";";
        }
        EV<<endl;
        EV<<sonId<<","<<sonRank<<','<<ThisDeviceInfo.EndDeviceRank<<endl;
    }

    if(strcmp("selfJudgeIfBackwardValid", inMsg->getFullName()) == 0){     
        if(networkedByFatherId != -2  && networkingToSonId == -2){
            lastAtionTime = inMsg->getLastActionTime() + interval;
            otherNode = getParentModule()->getSubmodule("node", canBeBackwardAndBackwardToFatherID);
            wmsg *msgcopy = broadcastJudgeIfBackwardValid->dup();
            PreparePacket(msgcopy, 0, lastAtionTime, controlPacketLength);
            msgcopy->setBackwardScore(deviceBackwardScore);
            Broadcast(msgcopy);
            //networkedByFatherId = -2;
            //if(!ifDead) sendDirect(msgcopy, otherNode, "port");
        }
        networkingToSonId = -2;
    }
}

void Node::EndDeviceInit()
{
    //getParentModule()->getSubmodule("node", ThisDeviceInfo.EndDeviceID)->par("fatherID") = -2;
    otherNode = getParentModule()->getSubmodule("node", this->getId() - 2);
    otherNode->par("fatherID") = -2;
    ThisDeviceInfo.EndDeviceID = this->getId() - 2;
    //ThisDeviceInfo.EndDeviceRank = 0;
    ThisDeviceInfo.EndDevicePriority = 0;
    //ThisDeviceInfo.EndDeviceBattery = maxBattery;
    ThisDeviceInfo.EndDevicePacketLength = dataPacketLength + controlPacketLength;
    deviceBackwardScore = maxBattery;
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
            wmsg *msgcopy = txpacket->dup();
            if(!ifDead) sendDirect(msgcopy, otherNode, "port");//otherNode->gate("port"));
        }
    }
}

int Node::SelectFatherEndDevice(){
    int neiborNum = ThisDeviceRoutingInfo.NeiborFatherEndDeviceNum;
    if(neiborNum == 0) return -2;
    double weight[neiborNum];
    for(int i = 0; i< neiborNum; ++i){
        weight[i] = (50*ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceBattery/maxBattery);// + 50*(maxTransDist - ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceDist)/maxTransDist);
    }
    double maxWeight = weight[0];
    int pos = 0;
    for(int i = 0; i< neiborNum; ++i){
        EV<<"father/weight//bat: "<<ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceID<<','<<weight[i]<<','<<ThisDeviceRoutingInfo.NeiborFatherEndDeviceInfo[i].FatherEndDeviceBattery<<';';
        if(weight[i] > maxWeight){
            maxWeight = weight[i];
            pos = i;
        } 
    }
    EV<<endl;
    return pos;
}

void Node::PreparePacket(wmsg *txpacket, int destID, double lastActionTime, int packetLength){
    txpacket->setDestID(destID);
    txpacket->setSourceID(ThisDeviceInfo.EndDeviceID);
    txpacket->setSourceRank(ThisDeviceInfo.EndDeviceRank);
    txpacket->setLastActionTime(lastActionTime);
    txpacket->setX(x);
    txpacket->setY(y);
    txpacket->setSourceBattery(ThisDeviceInfo.EndDeviceBattery);
    txpacket->setSourceNetworkedFlag(networkedFlag);
    txpacket->setNetworkingToSonFlagMsg(networkingToSonId);
    txpacket->setNetworkingToSonFlagSelectedId(networkedByFatherId);
    txpacket->setSourcePacketLength(packetLength);
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

void Node::OutputBattery(){
    lastAtionTime = simTime().dbl() + interval*10;
    // PreparePacket(selfCheck, 0, lastAtionTime);
    // rescheduleAt(lastAtionTime, selfCheck);
    const char* filenameOutputBattery = "OutputDeadDevice.txt";
    FILE *fileOutputBattery;
    //bool ifDead = getParentModule()->getSubmodule("node", i)->par("ifDead");
    fileOutputBattery = fopen(filenameOutputBattery, "a+");
    double nowTime = simTime().dbl();
    fprintf(fileOutputBattery, "%d,%f,%f,%d,%d\n", ThisDeviceInfo.EndDeviceID, nowTime, ThisDeviceInfo.EndDeviceBattery, round, ThisDeviceRoutingInfo.FatherEndDeviceID);
    fclose(fileOutputBattery);
}

void Node::OutputFatherID(){
    const char* filenameOutputFatherID = "OutputFatherID.txt";
    FILE *fileOutputFatherID;
    fileOutputFatherID = fopen(filenameOutputFatherID, "a+");
    fprintf(fileOutputFatherID, "%d,%f,%d,%d\n", simTime().dbl(), ThisDeviceInfo.EndDeviceID, ThisDeviceRoutingInfo.FatherEndDeviceID,ThisDeviceInfo.EndDeviceRank);
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
