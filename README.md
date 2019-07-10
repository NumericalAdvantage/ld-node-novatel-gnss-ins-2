# ld-node-novatel-gnss-ins-2

[![build status](https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2/badges/master/build.svg)](https://gitlab.com/link.developers/ld-node-novatel-gnss-ins-2/commits/master)

## Motivation and Context

An application to retrieve positional and inertial data from Novatel OEM4 and OEMV series of devices. It is suited ideally for OEMV receivers equipped with SPAN technology.

### Hardware setup
The setup consists of an OEMV receiver attached to a GNSS antenna and an Inertial Navigation System(INS) using an IMU. This application was tested against a setup with Novatel Propak V3 enclosure which consists of an OEMV3 receiver, connected to an HG1700 AG58 IMU and a GPS antenna.

### Configuration

#### BaudRate
The BaudRate is necessary for the novatel library to connect to the hardware. If you work with given Novatel Propak V3, **115200** is the right value.

#### SerialPortAddress
The SerialPortAddress is OS and machine dependent but on windows it is always a *COM* port and on unix systems it is always a *dev/ttyS~* port.

#### EnableIMU
If IMU data is also needed. Look at the supply to better understand what is available. 

#### GNSSRateHz
Defines the sampling frequency in **Hz** for the GPS data reception from the device. There is a limit of 5 Hz if EnableIMU is set to true. 20 Hz otherwise.

#### FileLogging
Basic information about the node is always recorded in a file even if this configuration is set to false. What this configuration actually does is, enables detailed logging for the data that is received from the novatel device. It would generate a pretty heavy log file when enabled, especially if IMU data is also being recorded. 

## The node in action
When connected to a map-gui:
![The node in action.](assets/test_drive_link2.png)

## Installation

```
conda install ld-node-novatel-gnss-ins-2
```

## Usage example

```
sudo ./ld-node-novatel-gnss-ins-2 --instance-file instance.json 

```
The `sudo` is important, because on Linux root privileges are required to access ports used for serial communication. 

## Specification

This node implements the following RFCs:
- 

## Contribution

Your help is very much appreciated. For more information, please see our [contribution guide](./CONTRIBUTING.md) and the [Collective Code Construction Contract](https://gitlab.com/link.developers/RFC/blob/master/001/README.md) (C4).

## Maintainers

- Ritwik Ghosh (maintainer, original author)
