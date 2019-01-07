Version 0.1 (05.01.2019)

Optimized for Rasperry Pi 3+ with LoRa GPS HAT for RPi v1.4 from Dragino

First version for the public. Following functions are implemented:
- Hardware detection and implementation for SEMTECH SX1276 LoRa Chip
- Continuous FANET RF monitoring (RF noise, received and transmitted packet) each 1 minute and each 15 minutes
- Reads weather data from MariaDB and send the information periodical over FANET (Service, Type = 4)
- Receives and stores position data in the MariaDB from each FANET unit (Tracking, Type = 1)
- Receives and stores name data in the MariaDB from each FANET unit (Name, Type = 2)
- Sends to each new detected FANET unit a welcome message (Message, Type = 3)
- Updates periodical a "routing" table in the MariaDB -> for future use

To-do:
- Code optimizing
- Receiver routine adapt to the OSI model
- ACK handling
- Forward handling
- Detection of corrupt received data
