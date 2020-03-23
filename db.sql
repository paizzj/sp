CREATE DATABASE IF NOT EXISTS `fch`;
USE `fch`;

CREATE TABLE `block` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `blockcount` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `tx` (
  `address` varchar(100) NOT NULL,
  `txid` varchar(200) NOT NULL,
  `height` bigint(20) NOT NULL,
  `data` text NOT NULL,
  `status` int(11) DEFAULT '0',
  PRIMARY KEY (`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `utxo` (
  `address` varchar(100) NOT NULL,
  `txid` varchar(200) NOT NULL,
  `vout` int(11) NOT NULL,
  `amount` varchar(50) NOT NULL,
  `height` bigint(20) NOT NULL,
  `coinbase` int(11) DEFAULT '0',
  `spent` int(11) DEFAULT '0',
  PRIMARY KEY (`address`,`txid`),
  KEY `i_utxo` (`txid`,`vout`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `history` (
  `txid` varchar(200) NOT NULL,
  `height` bigint(20) NOT NULL,
  `idx` int(11) NOT NULL,
  `tx_size` bigint(20) NOT NULL,
  `time` bigint(11) NOT NULL,
  `vins` longtext NOT NULL,
  `vouts` longtext NOT NULL,
  PRIMARY KEY (`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
