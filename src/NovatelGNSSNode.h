/*
 * This file is part of project link.developers/ld-node-novatel-gnss-ins-2.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef NOVATEL_GNSS_NODE_2_HPP
#define NOVATEL_GNSS_NODE_2_HPP

#include <memory>
#include <string>
#include <novatel/novatel.h>
#include <spdlog/spdlog.h>
#if defined(SPDLOG_VER_MAJOR) && SPDLOG_VER_MAJOR >= 1
#include <spdlog/sinks/basic_file_sink.h>	// for spdlog::basic_logger_mt(), required in spdlog v1.0 and higher
#endif

#define INSPVAB_DATA_RATE "0.01"     //Rate at which INS logs are requested (in seconds).
#define MAX_GNSS_DATA_RATE 5         //Maximum allowed data rate when INS is used.
#define MAX_GNSS_DATA_RATE_NO_IMU 20 //Maximum allowed data rate when INS isn't used.
#define IMU_RATE_HZ 100              //IMU Rate in Hz for HG1700 AG58 IMU. 
                                     //This cannot be adjusted. This value is
                                     //scalefactor for the CORRIMUDATAB or CORRIMUDATASB messages.
#define DEVICE_SETTLE_TIME 1000      //We pause in between commands we send to the device (in milliseconds)

namespace link_dev 
{
    class NovatelGNSSNode
    {
        private: 
            novatel::Novatel novatelDevice;     //Used to communicate with the novatel SPAN system. 
                                                //Source:  https://github.com/GAVLab/novatel
            int64_t m_dataRateHz = 0.0;         //Rate at which SPAN data is asked from the device.
            std::string m_serialCommAddr = "";  //Novatel requires serial communication.  
            int64_t m_baudRate = 0;             //Baud rate for serial communication.
            bool m_enableIMU = false;
            std::string m_logPath = "";
            bool m_logging = false;
            std::shared_ptr<spdlog::logger> m_logger;
            bool m_enableBadSolutionStatus = false;
            DRAIVE::Link2::NodeResources m_nodeResources;
            DRAIVE::Link2::NodeDiscovery m_nodeDiscovery;
            DRAIVE::Link2::OutputPin m_outputPin;
            
            bool ResetLogs(); //Initialization logic for the Novatel Device
            bool ConfigureRecording(); //Request data from novatel device
            bool SetCallBacks(); //Set callbacks for the data arrival from novatel.
            
            // Callback Handlers
            void BestPositionCallback(novatel::Position &posData, double &dData);
            void CorrImuCallback(novatel::CorrImu &corrIMUData, double &dReadTimestamp);

            //void BestGPSPositionCallback(novatel::Position &posData, double &dData);
            //void InsPositionVelocityAttitudeCallBack(novatel::InsPositionVelocityAttitude &posData, double &dReadTimestamp);
            

        public:

            NovatelGNSSNode(int64_t dataRateHz, std::string serialCommAddr,
                            bool enableIMU, bool enableBadSolutionStatus,
                            int64_t baudRate, std::string logPath,
                            bool logging, DRAIVE::Link2::NodeResources nodeResources,
                            DRAIVE::Link2::NodeDiscovery nodeDiscovery,
                            DRAIVE::Link2::OutputPin outputPin) : 
                            m_dataRateHz(dataRateHz),
                            m_serialCommAddr(serialCommAddr),
                            m_enableIMU(enableIMU),
                            m_enableBadSolutionStatus(enableBadSolutionStatus),
                            m_baudRate(baudRate),
                            m_logPath(logPath),
                            m_logging(logging),
                            m_nodeResources(nodeResources),
                            m_nodeDiscovery(nodeDiscovery),
                            m_outputPin(outputPin)
            {
                //Log file is appended with time in nanoseconds.
                m_logger = spdlog::basic_logger_mt("GNSS 2 LOG",
                           (logPath + "gnsslog_" + std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>
                           (std::chrono::system_clock::now().time_since_epoch()).count()) + ".txt").c_str());
                spdlog::set_pattern("[%H:%M:%S.%e] [%n] [%l] %v");
            }

            int8_t Init(); //Initialize connection with the novatel device using the library.
            void Cleanup(); //Use the novatel library to remove data subscriptions, h/w reset
        };
}

#endif 


//References:
//1. https://docs.novatel.com/OEM7/Content/PDFs/OEM7_Commands_Logs_Manual.pdf
//2. https://www.novatel.com/assets/Documents/Manuals/om-20000104.pdf
