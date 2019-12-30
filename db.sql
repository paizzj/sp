CREATE DATABASE IF NOT EXISTS `xsv`; 
USE `xsv`;

CREATE TABLE `block` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `blockcount` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `tx` (
  `sender` varchar(100) NOT NULL,
  `recipient` varchar(100) NOT NULL,
  `txid` varchar(200) NOT NULL,
  `height` bigint(20) NOT NULL,
  PRIMARY KEY (`recipient`,`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `utxo` (
  `address` varchar(100) NOT NULL,
  `txid` varchar(200) NOT NULL,
  `vout` int(11) NOT NULL,
  `amount` varchar(50) NOT NULL,
  `script` varchar(200) NOT NULL,
  `height` bigint(20) NOT NULL,
  PRIMARY KEY (`address`,`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
