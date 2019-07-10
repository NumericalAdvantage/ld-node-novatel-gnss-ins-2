/*
 * This file is part of project link.developers/ld-node-novatel-gnss-ins-2.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <thread>
#include <DRAIVE/Link2/OutputPin.hpp>
#include "NovatelGNSSNode.h"
#include "data_generated.h"

/*! \brief Device Initialization
*    Reset the device and Deregister all the logs
*   \return void
*/
bool link_dev::NovatelGNSSNode::ResetLogs() 
{
    try 
    {
        //Deregister all previously configured logs.
        //Reference: OEM7_Commands_Logs_Manual [Pg. 397]
        novatelDevice.SendCommand("UNLOGALL TRUE", true);
    }
    catch (std::exception ex) 
    {
        return false;
    }
    return true;
}

/*! \brief Set Callback for all the messages to be receive information from
 * the Novatel Device.
 * Set the callbacks for logging info, debug, error and warning data in
 *    addition to the following data:
 *    1. RAW IMU data
 *    2. Best Position data
 *   \return void0
 */
bool link_dev::NovatelGNSSNode::SetCallBacks() 
{
    try 
    {
        if(m_enableIMU)
        {
            /*Initialize the device callbacks for the Novatel Corrected IMU Messages*/
            novatelDevice.set_corr_imu_callback( 
                [this](novatel::CorrImu &imuData, double &dReadTimestamp) 
                { CorrImuCallback(imuData, dReadTimestamp); });
        }

        //Initialize the device callbacks for SPAN BESTPOS data. 
        novatelDevice.set_best_position_callback(
                [this](novatel::Position &pos, double &dReadTimestamp) 
                { BestPositionCallback(pos, dReadTimestamp); });

        /*
        //Initialize the device callbacks for the Novatel GPS
        novatelDevice.set_best_gps_position_callback(
                [this](novatel::Position &pos, double &dReadTimestamp) 
                { BestGPSPositionCallback(pos, dReadTimestamp); });
        //Initialize the device callbacks for INS position data.
        novatelDevice.set_ins_position_velocity_attitude_callback(
                [this](novatel::InsPositionVelocityAttitude &pos, double &dReadTimestamp) 
                {InsPositionVelocityAttitudeCallBack(pos, dReadTimestamp);}); 
        */
    }
    catch (std::exception ex) 
    {
        m_logger->error("An error occurred while configuring the Novatel Device: {0}", ex.what());
        return false;
    }
    return true;
}



/*! \brief Configure the messages to be recorded in the Novatel Device
    The messages that are recorded are:
     SNo    Message         Trigger     Frequency(Hz)
     1.     BESTPOSB        ONTIME      User Defined (Recommended:5 Hz)
     2.     CORRIMUDATAB    ONTIME      0.01 (only 1 message from INS is allowed at 100 Hz)
   \return bool
*/
bool link_dev::NovatelGNSSNode::ConfigureRecording() 
{
    try {
        //From reference manuals of Novatel, the max rate at which SPAN log BESTPOSB
        //can be requested is 20 Hz. However, if any INS log is also requested, then GPS
        //and SPAN logs can only be requested at 5 Hz.
        if(m_enableIMU)
        {
            if(m_dataRateHz <= MAX_GNSS_DATA_RATE)
            {
                //Reference: SPAN Technology for OEMV User Manual [Pg. 146]
                novatelDevice.ConfigureLogs("BESTPOSB ONTIME " + std::to_string(1.0 / m_dataRateHz));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                //Reference: SPAN Technology for OEMV User Manual [Pg. 155]
                novatelDevice.ConfigureLogs("CORRIMUDATAB ONTIME 0.01");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            else
            {
                throw std::invalid_argument("User configured data rate is over the permissible limits. Read the manual.");
            }
        }
        else
        {
            if(m_dataRateHz <= MAX_GNSS_DATA_RATE_NO_IMU)
            {
                //Reference: SPAN Technology for OEMV User Manual [Pg. 146]
                novatelDevice.ConfigureLogs("BESTPOSB ONTIME " + std::to_string(1.0 / m_dataRateHz));
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            else
            {
                throw std::invalid_argument("User configured data rate is over the permissible limits. Read the manual.");
            }
        }
        //novatelDevice.ConfigureLogs("BESTGPSPOSB ONTIME " + std::to_string(1.0 / _oConfiguration.GNSSRateHz)); //-- We don't use this data for anything.
        //std::stringstream insCommand; insCommand << "INSPVAB ONTIME "; insCommand << INSPVAB_DATA_RATE;
        //novatelDevice.ConfigureLogs(insCommand.str());
    }
    catch(std::exception ex)
    {
        m_logger->error("An error occured while configuring the logs. {0}", ex.what());
        return false;
    }
    return true;
}

/*! \brief Does the complete initialization to setup the reading of data
*    from the novatel device.
*    The order of the initialization is the following:
*    1. Connect to the novatel Device (return value -2 if failed)
*    3. Deregister all logs for the novatel Device (return value -3 if failed)
*    4. Set the Callbacks (return value -4 if failed)
*    5. Configure the according Recordings (return value -5 if failed)
*	If the initialization succeeds, 0 is returned.
*   \return int8_t
*/
int8_t link_dev::NovatelGNSSNode::Init()
{
    if(novatelDevice.Connect(m_serialCommAddr, m_baudRate)) 
    {
        m_logger->info("Successfully connected to the Novatel Device.");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    else 
    {
        m_logger->error("Failed to connect to the Novatel Device.");
        return -2;
    }

    if(ResetLogs()) 
    {
        m_logger->info("Reset all logs of Novatel device");
    }
    else 
    {
        m_logger->error("The device's logs could not be reset..");
        return -3;
    }

    if(SetCallBacks()) 
    {
        m_logger->info("Successfully set callbacks for the Novatel Device.");
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    else 
    {
        m_logger->error("Failed to set callbacks for the Novatel Device.");
        return -4;
    }

	if(ConfigureRecording()) 
    {
		m_logger->info("Successfully configured the recording of the Novatel Device.");
	}
	else 
    {
		m_logger->error("Failed to configure the recording of the Novatel Device.");
		return -5;
	}
	return 0;
}

/*! \brief cleanup for the Novatel device
*    1. Deregister all logs
*    2. Do a Hardware Reset
*    3. Disconnect the device
*   \return void
*/
void link_dev::NovatelGNSSNode::Cleanup()
{
    novatelDevice.UnlogAll();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    novatelDevice.HardwareReset();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    novatelDevice.Disconnect();
}

/*! \brief Callback for BESTPOS data
*    set the serializable message of GPS position and
*    publish it on Link bus
*    If the data is not available, check if the last received INSPVAB value is correct, and if yes, use that data instead.   
*    Note that we don't need to check if the last recorded IMU data is "too old". This is because of the far higher data rate
*    of IMU data compared to BESTPOS and the fact that we mark the IMU data "invalid" if the latest reading was not a "good solution"
*    as defined by NovAtel.
*   \return void
*/
void link_dev::NovatelGNSSNode::BestPositionCallback(novatel::Position &posData, double &dData) {
	
	if (posData.solution_status != novatel::SOL_COMPUTED) 
    {
        if(m_logging)
        {
            m_logger->warn("Best Position data is not Computed: SolutionStatus:{0}, PositionType:{1}", std::to_string(posData.solution_status), std::to_string(posData.position_type));
        }
        
        return;
    }
    
    if(m_logging)
    {
        m_logger->info("Best Position Message received: Lat:{0},Lon:{1},Alt:{2}", posData.latitude, posData.longitude, posData.height);
    }

    GPSMeasurementT gps{};
    gps.altitude = posData.height;
    gps.latitude = posData.latitude;
    gps.longitude = posData.longitude;
    gps.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>
                    (std::chrono::system_clock::now().time_since_epoch()).count();

    m_outputPin.push(gps, "GPSMeasurement");
}


/*! \brief Callback for CORRIMUDATAB data
*    set the serializable message for IMU acceleration data and
*    Gyroscope data and publish it.
*   \return void*/
void link_dev::NovatelGNSSNode::CorrImuCallback(novatel::CorrImu &corrIMUData, double &dReadTimestamp) 
{
	
	//Map measurement to the the right coordinate system and scale it
	double accY = -corrIMUData.LateralAcc * IMU_RATE_HZ;
	double accX = corrIMUData.LongitudinalAcc * IMU_RATE_HZ;
	double accZ = corrIMUData.VerticalAcc * IMU_RATE_HZ;
	double gyrX = corrIMUData.rollRate * IMU_RATE_HZ;
	double gyrZ = corrIMUData.yawRate * IMU_RATE_HZ;
	double gyrY = -corrIMUData.pitchRate * IMU_RATE_HZ;

    if(m_logging)
    {
	    m_logger->info("CORR IMU Message received at {0}:IMU ACC-X:{1},IMU ACC-Y:{2},IMU ACC-Z:{3}, IMU-GYRO-X:{4},IMU-GYRO-Y:{5},IMU-GYRO-Z:{6}",
                        corrIMUData.gps_millisecs, accX, accY, accZ, gyrX, gyrY, gyrZ);
    }

    AccDataT ad{};
    ad.accownX = accX; ad.accownY = accY; ad.accownZ = accZ;
    ad.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>
                   (std::chrono::system_clock::now().time_since_epoch()).count();
    m_outputPin.push(ad, "AccelerationData");
    
    GyrDataT gd{};
    gd.gyrownX = gyrX; gd.gyrownY = gyrY; gd.gyrownZ = gyrZ;
    gd.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>
                   (std::chrono::system_clock::now().time_since_epoch()).count();
    m_outputPin.push(gd, "GyroscopeData");

    return;
}


/*! \brief Callback for BESTGPSPOS data
*    set the serializable message of GPS position and
*    publish it on Link bus
*   \return void
*/
//Note that this callback is never actually called.
// void link_dev::Services::NovatelNode::BestGPSPositionCallback(Position &posData, double &dReadTimestamp)
// {
//     if (posData.solution_status != SOL_COMPUTED)
//     {
//         _logger.warn("Best GPS Position data is not Computed: SolutionStatus:{0}, PositionType:{1}", std::to_string(posData.solution_status), std::to_string(posData.position_type));
//         return;
//     }
//     _logger.info("Best GPS Message received: Lat:{0},Lon:{1},Alt:{2}", posData.latitude, posData.longitude, posData.height);
// }

/*
Callback for recording ins_position_velocity_attitude
*/
// void link_dev::Services::NovatelNode::InsPositionVelocityAttitudeCallBack(novatel::InsPositionVelocityAttitude &posData, double &dtimestamp) 
// {
//     if(posData.status != INS_SOLUTION_GOOD /*&& posData.status != INS_ALIGNMENT_COMPLETE*/)
//     {
//         _logger.warn("INS Position, Velocity, Attitude not computed: SolutionStatus:{0}", std::to_string(posData.status));
//         m_isIMUValid.store(false);
//         return;
//     }

//     _logger.info("IMU Position Message received: Lat:{0},Lon:{1},Alt:{2}", posData.latitude, posData.longitude, posData.height);

//     m_IMURecordedLatitude.store(posData.latitude);
//     m_IMURecordedLongitude.store(posData.longitude); 
//     m_IMURecordedAltitude.store(posData.height);
//     m_isIMUValid.store(true);
// }

