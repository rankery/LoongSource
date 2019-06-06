/* 帐号库 */
DROP TABLE IF EXISTS `account`;

CREATE TABLE `account` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '帐号ID',
  `name` char(36) NOT NULL COMMENT '帐号名',
  `psd` char(50) NOT NULL COMMENT '密码，一般为MD5',
  `privilege` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT 'GM权限',
  `login_status` tinyint(4) NOT NULL DEFAULT '0' COMMENT '登录状态',
  `worldname_crc` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '登陆的游戏世界',
  `forbid_mask` tinyint(4) UNSIGNED NOT NULL DEFAULT 0 comment '对应目前各种封停类型，掩码',
  `guard` tinyint(4) NOT NULL DEFAULT '0' COMMENT '是否防沉迷',
  `mibao` char(240) DEFAULT NULL COMMENT '密保',
  `ip` varchar(20) DEFAULT NULL COMMENT '上次登录ip',
  `time` datetime DEFAULT NULL COMMENT '上次登录time',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/* 反外挂日志 */

DROP TABLE IF EXISTS `game_guarder_log`;

CREATE TABLE `game_guarder_log` (                         
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,          
  `account_id` int(11) unsigned NOT NULL COMMENT '帐号ID',
  `name` varchar(32) NOT NULL,                            
  `kick_time` char(20) NOT NULL COMMENT '踢掉的时间',
  `seal` tinyint(1) NOT NULL COMMENT '封号',
  `error_code` int(11) unsigned NOT NULL COMMENT '原因',
  PRIMARY KEY (`id`)                                      
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/* 登陆日志 */

DROP TABLE IF EXISTS `login_log`;

CREATE TABLE `login_log` (                                   
 `accountID` int(11) unsigned NOT NULL COMMENT '帐号ID',  
 `accountName` varchar(32) NOT NULL,                        
 `ip` varchar(20) NOT NULL COMMENT '登陆的IP',           
 `action` varchar(6) NOT NULL COMMENT '动作',             
 `time` datetime DEFAULT NULL COMMENT '时间'              
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/* 游戏世界状态 */
DROP TABLE IF EXISTS `world_state`;
CREATE TABLE `world_state` (
  `worldid` bigint(11) NOT NULL DEFAULT '0' COMMENT '世界ID',
  `rolenum` int(11) NOT NULL DEFAULT '0' COMMENT '当前在线人数',
  `worldstate` tinyint(3) NOT NULL DEFAULT '0' COMMENT '(当前人数/最高负载)*100',
  `time` char(20) DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`worldid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='世界状态 不停的update';

DROP TABLE IF EXISTS `world_state_log`;
CREATE TABLE `world_state_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `worldid` bigint(11) NOT NULL DEFAULT '0' COMMENT '世界ID',
  `rolenum` int(11) NOT NULL DEFAULT '0' COMMENT '当前在线人数',
  `worldstate` tinyint(3) NOT NULL DEFAULT '0' COMMENT '(当前人数/最高负载)*100',
  `time` char(20) DEFAULT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`)  
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='世界状态 不停的insert';

/* gm封停游戏玩家指定游戏世界 记录表 */
DROP TABLE IF EXISTS `world_forbid`;
CREATE TABLE `world_forbid` (
  `accountid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '账号id',
  `worldname_crc` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏世界名的crc',
  PRIMARY KEY (`accountid`,`worldname_crc`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='记录gm工具封停账号';

##2009-09-07
#增加ip封停黑名单
DROP TABLE IF EXISTS `black_list`;
CREATE TABLE `black_list` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(20) DEFAULT NULL COMMENT '被封的ip',
  PRIMARY KEY (`id`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='黑名单';

##2009-10-10
#添加防沉迷时间表
CREATE TABLE `fatigue_time` (                                           
                `accountname_crc` int(11) UNSIGNED NOT NULL COMMENT '帐号名crc',            
                `acc_online_time` int(11) DEFAULT '0' COMMENT '累计在线时间',   
                `acc_offline_time` int(11) DEFAULT '0' COMMENT '累计离线时间',  
                PRIMARY KEY (`accountname_crc`)                                       
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;