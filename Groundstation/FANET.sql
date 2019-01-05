-- MySQL dump 10.16  Distrib 10.1.37-MariaDB, for debian-linux-gnueabihf (armv8l)
--
-- Host: localhost    Database: FANET
-- ------------------------------------------------------
-- Server version	10.1.37-MariaDB-0+deb9u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `messages_send`
--

DROP TABLE IF EXISTS `messages_send`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `messages_send` (
  `idmessages_send` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` int(11) DEFAULT NULL,
  `subheader` smallint(6) DEFAULT NULL,
  `s_address` varchar(10) DEFAULT NULL,
  `d_address` varchar(10) DEFAULT NULL,
  `ack_set` smallint(6) DEFAULT NULL,
  `ack_status` smallint(6) DEFAULT NULL,
  `send_events` smallint(6) DEFAULT NULL,
  `message_type` smallint(6) DEFAULT NULL,
  `message` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`idmessages_send`)
) ENGINE=InnoDB AUTO_INCREMENT=83 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `object_name`
--

DROP TABLE IF EXISTS `object_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `object_name` (
  `idobject_name` varchar(10) NOT NULL,
  `name` varchar(255) DEFAULT NULL,
  `first_seen` int(11) DEFAULT NULL,
  `last_seen` int(11) DEFAULT NULL,
  `last_welcom_message` int(11) DEFAULT NULL,
  PRIMARY KEY (`idobject_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='Stores the name from message type 2';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `object_tracking`
--

DROP TABLE IF EXISTS `object_tracking`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `object_tracking` (
  `idobject_tracking` int(11) NOT NULL AUTO_INCREMENT,
  `time_stamp` int(11) DEFAULT NULL,
  `idobject_name` varchar(10) NOT NULL,
  `P_RSSI` smallint(6) DEFAULT NULL,
  `P_SNR` float DEFAULT NULL,
  `coding_rate` varchar(5) DEFAULT NULL,
  `freq_dev` int(11) DEFAULT NULL,
  `latitude` float DEFAULT NULL,
  `longitude` float DEFAULT NULL,
  `online_tracking` bit(1) DEFAULT NULL,
  `aircraft_typ` smallint(6) DEFAULT NULL,
  `alt` smallint(6) DEFAULT NULL,
  `speed` float DEFAULT NULL,
  `climb` float DEFAULT NULL,
  `heading` float DEFAULT NULL,
  `turn_rate` float DEFAULT NULL,
  `distance` mediumint(9) DEFAULT NULL,
  PRIMARY KEY (`idobject_tracking`)
) ENGINE=InnoDB AUTO_INCREMENT=18904 DEFAULT CHARSET=utf8mb4 COMMENT='Records the tracking of message type 1';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `routing_table`
--

DROP TABLE IF EXISTS `routing_table`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `routing_table` (
  `idrouting_table` int(11) NOT NULL AUTO_INCREMENT,
  `address` varchar(10) DEFAULT NULL,
  `subnet` varchar(10) DEFAULT NULL,
  `next_hop` varchar(10) DEFAULT NULL,
  `metrik` int(11) DEFAULT NULL,
  `last_seen` int(11) DEFAULT NULL,
  `snr` float DEFAULT NULL,
  PRIMARY KEY (`idrouting_table`)
) ENGINE=InnoDB AUTO_INCREMENT=160 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `system_data_15min`
--

DROP TABLE IF EXISTS `system_data_15min`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `system_data_15min` (
  `idsystem_data_15min` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` int(11) DEFAULT NULL,
  `rx_rssi_avg` float DEFAULT NULL,
  `rx_rssi_max` int(11) DEFAULT NULL,
  `rx_packet` int(11) DEFAULT NULL,
  `tx_packet` int(11) DEFAULT NULL,
  `tx_time` int(11) DEFAULT NULL,
  PRIMARY KEY (`idsystem_data_15min`)
) ENGINE=InnoDB AUTO_INCREMENT=724 DEFAULT CHARSET=utf8mb4 COMMENT='Records different system data for analyses';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `system_data_1min`
--

DROP TABLE IF EXISTS `system_data_1min`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `system_data_1min` (
  `idsystem_data_1min` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` int(11) DEFAULT NULL,
  `rx_rssi_avg` float DEFAULT NULL,
  `rx_rssi_max` int(11) DEFAULT NULL,
  `rx_packet` int(11) DEFAULT NULL,
  `tx_packet` int(11) DEFAULT NULL,
  `tx_time` int(11) DEFAULT NULL,
  PRIMARY KEY (`idsystem_data_1min`)
) ENGINE=InnoDB AUTO_INCREMENT=132131 DEFAULT CHARSET=utf8mb4 COMMENT='Records different system data for analyses';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `weather_stations`
--

DROP TABLE IF EXISTS `weather_stations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `weather_stations` (
  `_id` varchar(32) NOT NULL,
  `name` tinytext,
  `short` tinytext,
  `longitude` float DEFAULT NULL,
  `latitude` float DEFAULT NULL,
  `alt` int(11) DEFAULT NULL,
  PRIMARY KEY (`_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `weather_values`
--

DROP TABLE IF EXISTS `weather_values`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `weather_values` (
  `idweather_values` int(11) NOT NULL AUTO_INCREMENT,
  `_id_stations` varchar(45) DEFAULT NULL,
  `time` int(11) DEFAULT NULL,
  `temp` float DEFAULT NULL,
  `hum` float DEFAULT NULL,
  `pres` float DEFAULT NULL,
  `w_avg` float DEFAULT NULL,
  `w_max` float DEFAULT NULL,
  `w_dir` int(11) DEFAULT NULL,
  PRIMARY KEY (`idweather_values`)
) ENGINE=InnoDB AUTO_INCREMENT=103888 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-01-05 21:20:54
