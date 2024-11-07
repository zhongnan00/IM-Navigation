/**
 * @file NdiSampleApp.cpp
 * @author chao.liu (chao.liu2@medtronic.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "NdiSampleApp.h"

using namespace app;

NdiSampleApp::NdiSampleApp(/* args */)
{
}

NdiSampleApp::~NdiSampleApp()
{
}

/*
 * @brief Equivalent of std::filesystem::exists() that works with -std=c++11
 */
bool NdiSampleApp::fileExists(const std::string &Filename)
{
    return ACCESS(Filename.c_str(), 0) == 0;
}

/**
 * @brief There's no standard cross platform sleep() keystroke detection
 */
int NdiSampleApp::detectKeystroke()
{
    int bytesWaiting;
    ioctl(0, FIONREAD, &bytesWaiting); //stdin=0
    return bytesWaiting;

}

/**
 * @brief There's no standard cross platform sleep() method prior to C++11
 */
void NdiSampleApp::sleepSeconds(unsigned numSeconds)
{

    sleep(numSeconds); // sleep(sec)

}

/**
 * @brief Prints a debug message if a method call failed.
 * @details To use, pass the method name and the error code returned by the method.
 *          Eg: onErrorPrintDebugMessage("capi.initialize()", capi.initialize());
 *          If the call succeeds, this method does nothing.
 *          If the call fails, this method prints an error message to stdout.
 */
void NdiSampleApp::onErrorPrintDebugMessage(std::string methodName, int errorCode)
{
    if (errorCode < 0)
    {
        std::cout << methodName << " failed: " << capi.errorToString(errorCode) << std::endl;
    }
}

/**
* @brief Returns the string: "[tool.id] s/n:[tool.serialNumber]" used in CSV output
*/
std::string NdiSampleApp::getToolInfo(std::string toolHandle)
{
    // Get the port handle info from PHINF
    PortHandleInfo info = capi.portHandleInfo(toolHandle);

    // Return the ID and SerialNumber the desired string format
    std::string outputString = info.getToolId();
    outputString.append(" s/n:").append(info.getSerialNumber());
    return outputString;
}


/**
* @brief Returns a string representation of the data in CSV format.
* @details The CSV format is: "Frame#,ToolHandle,Face,TransformStatus,q0,qx,qy,qz,tx,ty,tz,error,#markers,[Marker1:status,x,y,z],[Marker2..."
*/
std::string NdiSampleApp::toolDataToCSV(const ToolData& toolData)
{
    std::stringstream stream;
    stream << std::setprecision(toolData.PRECISION) << std::setfill('0');
    stream << "" << static_cast<unsigned>(toolData.frameNumber) << ","
           << "Port:" << static_cast<unsigned>(toolData.transform.toolHandle) << ",";
    stream << static_cast<unsigned>(toolData.transform.getFaceNumber()) << ",";

    if (toolData.transform.isMissing())
    {
        stream << "Missing,,,,,,,,";
    }
    else
    {
        stream << TransformStatus::toString(toolData.transform.getErrorCode()) << ","
               << toolData.transform.q0 << "," << toolData.transform.qx << "," << toolData.transform.qy << "," << toolData.transform.qz << ","
               << toolData.transform.tx << "," << toolData.transform.ty << "," << toolData.transform.tz << "," << toolData.transform.error;
    }

    // Each marker is printed as: status,tx,ty,tz
    stream << "," << toolData.markers.size();
    for ( int i = 0; i < toolData.markers.size(); i++)
    {
        stream << "," << MarkerStatus::toString(toolData.markers[i].status);
        if (toolData.markers[i].status == MarkerStatus::Missing)
        {
            stream << ",,,";
        }
        else
        {
            stream << "," << toolData.markers[i].x << "," << toolData.markers[i].y << "," << toolData.markers[i].z;
        }
    }
    return stream.str();
}


/**
 * @brief Write tracking data to a CSV file in the format: "#Tools,ToolInfo,Frame#,[Tool1],Frame#,[Tool2]..."
 * @details It's worth noting that the number lines in the file does not necessarily match the number of frames collected.
 *          NDI measurement systems support different types of tools: passive, active, and active-wireless.
 *          Because different tool types are detected in different physical ways, each tool type has a separate frame for
 *          collecting data for all tools of that type. Each line of the file has the same number of tools, but each tool
 *          may have a different frame number that corresponds to its tool type.
 * @param fileName The file to write to.
 * @param numberOfLines The number of lines to write.
 * @param enabledTools ToolData storing the serial number for each enabled tool.
 */
void NdiSampleApp::writeCSV(std::string fileName, int numberOfLines, std::vector<ToolData>& enabledTools)
{
    // Print header information to the first line of the output file
    std::cout << std::endl << "Writing CSV file..." << std::endl;
    std::ofstream csvFile(fileName.c_str());
    csvFile << "#Tools";

    // Loop to gather tracking data and write to the file
    int linesWritten = 0;
    int previousFrameNumber = 0; // use this variable to avoid printing duplicate data with BX
    while (linesWritten < numberOfLines)
    {
        // Get new tool data using BX2
        std::vector<ToolData> newToolData = apiSupportsBX2 ? capi.getTrackingDataBX2("--6d=tools --3d=tools --sensor=none --1d=buttons") :
                                                             capi.getTrackingDataBX(TrackingReplyOption::TransformData | TrackingReplyOption::AllTransforms);

        // Update enabledTools array with new data
        for (int t = 0; t < enabledTools.size(); t++)
        {
            for (int j = 0; j < newToolData.size(); j++)
            {
                if (enabledTools[t].transform.toolHandle == newToolData[j].transform.toolHandle)
                {
                    // Copy the new tool data
                    newToolData[j].toolInfo = enabledTools[t].toolInfo; // keep the serial number
                    enabledTools[t] = newToolData[j]; // use the new data
                }
            }
        }

        // If we're using BX2 there's extra work to do because BX2 and BX use opposite philosophies.
        // BX will always return data for all enabled tools, but some of the data may be old: #linesWritten == # BX commands
        // BX2 never returns old data, but cannot guarantee new data for all enabled tools with each call: #linesWritten <= # BX2 commands
        // We want a CSV file with data for all enabled tools in each line, but this requires multiple BX2 calls.
        if (apiSupportsBX2)
        {
            // Count the number of tools that have new data
            int newDataCount = 0;
            for (int t = 0; t < enabledTools.size(); t++)
            {
                if (enabledTools[t].dataIsNew)
                {
                    newDataCount++;
                }
            }

            // Send another BX2 if some tools still have old data
            if (newDataCount < enabledTools.size())
            {
                continue;
            }
        }
        else
        {
            if (previousFrameNumber == enabledTools[0].frameNumber)
            {
                // If the frame number didn't change, don't print duplicate data to the CSV, send another BX
                continue;
            }
            else
            {
                // This frame number is different, so we'll print a line to the CSV, but remember it for next time
                previousFrameNumber = enabledTools[0].frameNumber;
            }
        }

        // If this is the first line of the CSV, print labels for tool and marker data
        if (linesWritten == 0)
        {
            for (int t = 0; t < enabledTools.size(); t++)
            {
                csvFile << ",ToolInfo,Frame#,PortHandle,Face#,TransformStatus,Q0,Qx,Qy,Qz,Tx,Ty,Tz,Error,#Markers";
                for (int m = 0; m < enabledTools[t].markers.size(); m++)
                {
                    csvFile << ",Marker" << m << ".Status,Tx,Ty,Tz";
                }
            }
            csvFile << std::endl;
        }


        // Print a line of the CSV file if all enabled tools have new data
        csvFile << std::dec << enabledTools.size();
        for (int t = 0; t < enabledTools.size(); t++)
        {
            csvFile << "," << enabledTools[t].toolInfo << "," << toolDataToCSV(enabledTools[t]);
            enabledTools[t].dataIsNew = false; // once printed, the data becomes "old"
        }
        csvFile << std::endl;
        linesWritten++;
    }
}


/**
 * @brief Prints a ToolData object to stdout
 * @param toolData The data to print
 */
void NdiSampleApp::printToolData(const ToolData& toolData)
{
    if (toolData.systemAlerts.size() > 0)
    {
        std::cout << "[" << toolData.systemAlerts.size() << " alerts] ";
        for (int a = 0; a < toolData.systemAlerts.size(); a++)
        {
            std::cout << toolData.systemAlerts[a].toString() << std::endl;
        }
    }

    if (toolData.buttons.size() > 0)
    {
        std::cout << "[buttons: ";
        for (int b = 0; b < toolData.buttons.size(); b++)
        {
            std::cout << ButtonState::toString(toolData.buttons[b]) << " ";
        }
        std::cout << "] ";
    }
    std::cout << toolDataToCSV(toolData) << std::endl;
}

/**
 * @brief Put the system into tracking mode, and poll a few frames of data.
 */
void NdiSampleApp::printTrackingData()
{
    // Start tracking, output a few frames of data, and stop tracking

    for (int i = 0; i < 10; i++)
    {
        // Demonstrate TX command: ASCII command sent, ASCII reply received
        std::cout << capi.getTrackingDataTX() << std::endl;

        // Demonstrate BX or BX2 command
        std::vector<ToolData> toolData =  apiSupportsBX2 ? capi.getTrackingDataBX2() : capi.getTrackingDataBX();

        // Print to stdout in similar format to CSV
        std::cout << "[alerts] [buttons] Frame#,ToolHandle,Face#,TransformStatus,Q0,Qx,Qy,Qz,Tx,Ty,Tz,Error,#Markers,State,Tx,Ty,Tz" << std::endl;
        for (int i = 0; i < toolData.size(); i++)
        {
            printToolData(toolData[i]);
        }
    }
}


/*
 * @brief Streams a command using insecure or secure TCP
 */
void NdiSampleApp::streamTcp()
{
    std::string streamId = useEncryption ? "tlsStream1" : "tcpStream1";
    capi.startStreaming("TX 0801", streamId, useEncryption ? Protocol::SecureTCP : Protocol::TCP, cipher);

    // Wait until a keystroke, or if there's an "ERROR" in the stream
    while (detectKeystroke() == 0 && streamedReply.rfind( "ERROR", 0 ) != 0 )
    {
        streamedReply = capi.readStream(streamId);
    }
    capi.stopStreaming(streamId, useEncryption ? Protocol::SecureTCP : Protocol::TCP );
}



/*
 * @brief Streams a command using insecure or secure UDP
 */
void NdiSampleApp::streamUdp()
{
    std::string streamId = useEncryption ? "dtlsStream1" : "udpStream1";
    capi.startStreaming("TX 0801", streamId, useEncryption ? Protocol::SecureUDP : Protocol::UDP, cipher);

    // Wait until a keystroke, or if there's an "ERROR" in the stream
    while ( detectKeystroke() == 0 && streamedReply.rfind( "ERROR", 0 ) != 0 )
    {
        streamedReply = capi.readStream(streamId);
    }
    capi.stopStreaming(streamId, useEncryption ? Protocol::SecureUDP : Protocol::UDP );
}



/**
 * @brief Initialize and enable loaded tools. This is the same regardless of tool type.
 */
void NdiSampleApp::initializeAndEnableTools(std::vector<ToolData>& enabledTools)
{
    std::cout << std::endl << "Initializing and enabling tools..." << std::endl;

    // Initialize and enable tools
    std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
    for (int i = 0; i < portHandles.size(); i++)
    {
        onErrorPrintDebugMessage("capi.portHandleInitialize()", capi.portHandleInitialize(portHandles[i].getPortHandle()));
        onErrorPrintDebugMessage("capi.portHandleEnable()", capi.portHandleEnable(portHandles[i].getPortHandle()));
    }

    // Print all enabled tools
    portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::Enabled);
    for (int i = 0; i < portHandles.size(); i++)
    {
        std::cout << portHandles[i].toString() << std::endl;
    }

    // Lookup and store the serial number for each enabled tool
    for (int i = 0; i < portHandles.size(); i++)
    {
        enabledTools.push_back(ToolData());
        enabledTools.back().transform.toolHandle = (uint16_t)capi.stringToInt(portHandles[i].getPortHandle());
        enabledTools.back().toolInfo = getToolInfo(portHandles[i].getPortHandle());
    }
}



/**
 * @brief Loads a tool from a tool definition file (.rom)
 */
void NdiSampleApp::loadTool(const char* toolDefinitionFilePath)
{
    // Request a port handle to load a passive tool into
    int portHandle = capi.portHandleRequest();
    onErrorPrintDebugMessage("capi.portHandleRequest()", portHandle);

    // Load the .rom file using the previously obtained port handle
    capi.loadSromToPort(toolDefinitionFilePath, portHandle);
}

/**
 * @brief Demonstrate detecting active tools.
 * @details Active tools are connected through a System Control Unit (SCU) with physical wires.
 */
void NdiSampleApp::configureActiveTools(std::string scuHostname)
{
    // Setup the SCU connection for demonstrating active tools
    std::cout << std::endl << "Configuring Active Tools - Setup SCU Connection" << std::endl;
    onErrorPrintDebugMessage("capi.setUserParameter()", capi.setUserParameter("Param.Connect.SCU Hostname", scuHostname));
    std::cout << capi.getUserParameter("Param.Connect.SCU Hostname") << std::endl;

    // Wait a few seconds for the SCU to detect any wired tools plugged in
    std::cout << std::endl << "Demo Active Tools - Detecting Tools..." << std::endl;
    sleepSeconds(2);

    // Print all port handles
    std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
    for (int i = 0; i < portHandles.size(); i++)
    {
        std::cout << portHandles[i].toString() << std::endl;
    }
}



/**
 * @brief Demonstrate loading an active wireless tool.
 * @details Active wireless tools are battery powered and emit IR in response to a chirp from the illuminators.
 */
void NdiSampleApp::configureActiveWirelessTools()
{
    // Load an active wireless tool definitions from a .rom files
    std::cout << std::endl << "Configuring an Active Wireless Tool - Loading .rom File..." << std::endl;
    loadTool("sroms/active-wireless.rom");
}

/**
 * @brief Demonstrate loading dummy tools of each tool type.
 * @details Dummy tools are used to report 3Ds in the absence of real tools.
 *          Dummy tools should not be loaded with regular tools of the same type.
 *          TSTART will fail if real and dummy tools are enabled simultaneously.
 */
void NdiSampleApp::configureDummyTools()
{
    std::cout << std::endl << "Loading passive, active-wireless, and active dummy tools..." << std::endl;
    onErrorPrintDebugMessage("capi.loadPassiveDummyTool()", capi.loadPassiveDummyTool());
    onErrorPrintDebugMessage("capi.loadActiveWirelessDummyTool()", capi.loadActiveWirelessDummyTool());
    onErrorPrintDebugMessage("capi.loadActiveDummyTool()", capi.loadActiveDummyTool());
}

/**
 * @brief Demonstrate getting/setting user parameters.
 */
void NdiSampleApp::configureUserParameters()
{
    std::cout << capi.getUserParameter("Param.User.String0") << std::endl;
    onErrorPrintDebugMessage("capi.setUserParameter(Param.User.String0, customString)", capi.setUserParameter("Param.User.String0", "customString"));
    std::cout << capi.getUserParameter("Param.User.String0") << std::endl;
    onErrorPrintDebugMessage("capi.setUserParameter(Param.User.String0, emptyString)", capi.setUserParameter("Param.User.String0", ""));
}

/**
 * @brief Sets the user parameter "Param.Simulated Alerts" to test communication of system alerts.
 * @details This method does nothing if simulatedAlerts is set to 0x00000000.
 */
void NdiSampleApp::simulateAlerts(uint32_t simulatedAlerts )
{
    // Simulate alerts if any were requested
    if (simulatedAlerts > 0x0000)
    {
        std::cout << std::endl << "Simulating system alerts..." << std::endl;
        std::stringstream stream;
        stream << simulatedAlerts;
        onErrorPrintDebugMessage("capi.setUserParameter(Param.Simulated Alerts, alerts)", capi.setUserParameter("Param.Simulated Alerts", stream.str()));
        std::cout << capi.getUserParameter("Param.Simulated Alerts") << std::endl;
    }
}

/**
 * @brief Determines whether an NDI device supports the BX2 command by looking at the API revision
 */
void NdiSampleApp::determineApiSupportForBX2()
{
    // Lookup the API revision
    std::string response = capi.getApiRevision();

    // Refer to the API guide for how to interpret the APIREV response
    char deviceFamily = response[0];
    int majorVersion = capi.stringToInt(response.substr(2,3));

    // As of early 2017, the only NDI device supporting BX2 is the Vega
    // Vega is a Polaris device with API major version 003
    if ( deviceFamily == 'G' && majorVersion >= 3)
    {
        apiSupportsBX2 = true;
        apiSupportsStreaming = true;
    }
}

void NdiSampleApp::printHelp()
{
    std::cout << "CAPIsample Ver " << capi.getVersion() << std::endl
        << "usage: ./capisample <hostname> --tools=[file1.rom],[file2.rom] [args]" << std::endl
        << "or   : ./capisample <input gbf file> <output txt file>" << std::endl
        << "where:" << std::endl
        << "    <hostname>      The measurement device's hostname, IP address, or serial port." << std::endl
        << "    --tools=[file1.rom],[file2.rom]...  A comma delimited list of tools to load." << std::endl
        << "    [args]          (optional) Any other arguments such as tools to load, or SCU to connect to." << std::endl
        << "    <input gbf file> a file that contains GBF data recorded by ToolBox." << std::endl
        << "    <output txt file> destination output file to contain the text representation of the input GBF file data." << std::endl
        << "example hostnames:" << std::endl
        << "    Connecting to device by IP address: 169.254.8.50" << std::endl
        << "    Connecting to device by hostname: P9-B0103.local" << std::endl
        << "    Connecting to serial port varies by operating system:" << std::endl
        << "        COM10 (Windows), /dev/ttyUSB0 (Linux), /dev/cu.usbserial-001014FA (Mac)" << std::endl
        << "Optional arguments:" << std::endl
#ifdef OPENSSL
        << "    --cipher=[ciphersuite] Limit OpenSSL to use a single TLS/DTLS ciphersuite. Only valid when --encrypted argument is also passed. For example --cipher=ECDHE-RSA-CAMELLIA256-SHA384." << std::endl
        << "    --encrypted Encrypt communcation with the device. Can specify ciphersuite with --cipher but not required." << std::endl
#endif
        << "    --help Displays the help information." << std::endl
        << "    --scu=[scu_hostname] A System Control Unit (SCU) hostname, used to connect active tools." << std::endl
        << "    --udp Stream data over UDP instead of TCP (default)" << std::endl;
}
