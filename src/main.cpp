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
#include "NovatelNode.h"

int main(int argc, char** argv)
{
    try {
        // load the node resources for our subscriber
        DRAIVE::Link2::NodeResources nodeResources { "l2spec:/link_dev/ld-node-novatel-gnss-ins-2", argc, argv };
        
        // announce that our node is "online"
        DRAIVE::Link2::NodeDiscovery nodeDiscovery { nodeResources };
        
        // create a blocking signal handler
        DRAIVE::Link2::SignalHandler signalHandler {};
        signalHandler.setReceiveSignalTimeout(-1);
        
        // if signal is not shutdown
        while (signalHandler.receiveSignal() != LINK2_SIGNAL_INTERRUPT)
            ;
        return link_dev::Services::NovatelNode{argc, argv}.run();        
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
