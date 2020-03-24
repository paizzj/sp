CREATE DATABASE IF NOT EXISTS `fch`;
USE `fch`;

CREATE TABLE `block` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `blockcount` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO block (blockcount) VALUES (0);

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
  `tx_size` int(11) NOT NULL,
  `time` bigint(11) NOT NULL,
  PRIMARY KEY (`txid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `vins` (
  `txid` varchar(200) NOT NULL,
  `i` int(11) NOT NULL,
  `hash` varchar(200) NOT NULL,
  `vout` int(11) NOT NULL,
  PRIMARY KEY (`txid`,`i`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `vouts` (
  `txid` varchar(200) NOT NULL,
  `address` varchar(200) NOT NULL,
  `n` int(11) NOT NULL,
  `value` varchar(100) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY (`txid`,`address`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
