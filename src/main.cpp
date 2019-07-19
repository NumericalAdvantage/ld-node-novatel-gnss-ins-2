/*
 * This file is part of project link.developers/ld-node-novatel-gnss-ins-2.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include <iostream>
#include <DRAIVE/Link2/NodeDiscovery.hpp>
#include <DRAIVE/Link2/NodeResources.hpp>
#include <DRAIVE/Link2/SignalHandler.hpp>
#include <DRAIVE/Link2/ConfigurationNode.hpp>
#include <DRAIVE/Link2/OutputPin.hpp>
#include "NovatelGNSSNode.h"

int main(int argc, char** argv)
{
    try 
    {
        // load the node resources for our subscriber
        DRAIVE::Link2::NodeResources nodeResources{"l2spec:/link_dev/ld-node-novatel-gnss-ins-2", argc, argv};
        // announce that our node is "online"
        DRAIVE::Link2::NodeDiscovery nodeDiscovery{nodeResources};
        
        DRAIVE::Link2::ConfigurationNode rootNode = nodeResources.getUserConfiguration();
        DRAIVE::Link2::OutputPin outputPin{nodeDiscovery, nodeResources, "GNSS2_output_pin"};
       
        DRAIVE::Link2::SignalHandler signalHandler {};
        signalHandler.setReceiveSignalTimeout(-1);

        link_dev::NovatelGNSSNode node{rootNode.getInt("GNSSRateHz"),
                                       rootNode.getString("SerialPortAddress"),
                                       rootNode.getBoolean("EnableIMU"),
                                       rootNode.getBoolean("EnableSolutionStatus"),
                                       rootNode.getInt("BaudRate"),
                                       rootNode.getString("LogPath"),
                                       rootNode.getBoolean("FileLogging"),
                                       nodeResources, nodeDiscovery, outputPin};
        if(node.Init() < 0)
        {
            return 1;        
        }

        while(signalHandler.receiveSignal() != LINK2_SIGNAL_INTERRUPT);
        
        node.Cleanup();            
        
        return 0;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}