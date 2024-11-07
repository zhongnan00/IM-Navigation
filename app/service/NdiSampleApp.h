/**
 * @file NdiSampleApp.h
 * @author chao.liu (chao.liu2@medtronic.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#define ACCESS access
#include <unistd.h>     // for POSIX sleep(sec), and access()
#include <sys/ioctl.h>

#include <iostream>
#include <vector>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>

#include "../CombinedAPI/library/include/CombinedApi.h"
#include "../CombinedAPI/library/include/PortHandleInfo.h"
#include "../CombinedAPI/library/include/ToolData.h"


namespace app{
    
class NdiSampleApp
{

public:
    NdiSampleApp(/* args */);
    ~NdiSampleApp();

    bool fileExists(const std::string &Filename);

    int  detectKeystroke();

    void sleepSeconds(unsigned numSeconds);

    void onErrorPrintDebugMessage(std::string methodName, int errorCode);

    std::string getToolInfo(std::string toolHandle);

    std::string toolDataToCSV(const ToolData& toolData);

    void writeCSV(std::string fileName, int numberOfLines, std::vector<ToolData>& enabledTools);

    void printToolData(const ToolData& toolData);

    void printTrackingData();

    void streamTcp();

    void streamUdp();

    void initializeAndEnableTools(std::vector<ToolData>& enabledTools);

    void loadTool(const char* toolDefinitionFilePath);

    void configureActiveTools(std::string scuHostname);

    void configureActiveWirelessTools();

    void configureDummyTools();

    void configureUserParameters();

    void simulateAlerts(uint32_t simulatedAlerts = 0x00000000);

    void determineApiSupportForBX2();

    void printHelp();

private:
    CombinedApi capi;

    bool apiSupportsBX2 = false;
    bool apiSupportsStreaming = false;
    bool useEncryption = false;
    bool useUDP = false;

    std::string cipher = "";
    std::string streamedReply = "";     ///* The streamed reply, may be used to quit streaming if there is an ERROR */

};



}