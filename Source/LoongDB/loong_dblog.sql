CREATE TABLE `log_money` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`CmdID` int(10) unsigned NOT NULL COMMENT '操作',
	`ConType` tinyint(3) NOT NULL COMMENT '容器类型',
	`Num` bigint(10) NOT NULL COMMENT '数量(+：表示存入；-：表示取出)',
	`TotalNum` bigint(20) NOT NULL COMMENT '指定类型容器中总量',
	`RoleIDRel` int(10) unsigned NOT NULL default '4294967295' COMMENT '相关联角色ID',
	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '背包、仓库金钱变化(单位：银)';
	
	
CREATE TABLE `log_yuanbao` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`CmdID` int(10) unsigned NOT NULL COMMENT '操作',
	`ConType` tinyint(3) NOT NULL COMMENT '容器类型',
	`Num` int(10) NOT NULL COMMENT '数量(+：表示存入；-：表示取出)',
	`TotalNum` int(20) NOT NULL COMMENT '指定类型容器中总量',
	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '背包、百宝袋元宝变化';


CREATE TABLE `log_exvolume` (                                       
                `ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',   
                `AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',           
                `RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',              
                `CmdID` int(10) unsigned NOT NULL COMMENT '操作',                 
                `Num` int(10) NOT NULL COMMENT '数量(+：表示存入；-：表示取出)',  
                `TotalNum` int(20) NOT NULL COMMENT '总量',         
                `Time` datetime NOT NULL COMMENT '时间',                          
                PRIMARY KEY (`ID`)                                                
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='赠点变化';
	

CREATE TABLE `log_time_stat` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`BagSilver` bigint(10) NOT NULL COMMENT '背包银子数量',
	`WareSilver` bigint(20) NOT NULL COMMENT '角色仓库银子总量',
	`BagYB` int(10) NOT NULL COMMENT '背包元宝数量',
	`BaiBaoYB` int(10) NOT NULL COMMENT '角色仓库元宝总量',
	`ExVolume` int(10) NOT NULL COMMENT '玩家角色赠点数量',
	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '在线玩家定时金钱元宝记录';
	
	
CREATE TABLE `log_item` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	
	`RoleID` int(10) unsigned NOT NULL COMMENT 'role id',
	`CmdID` int(10) unsigned NOT NULL COMMENT '操作',
	`TypeID` int(10) unsigned NOT NULL COMMENT 'type id',	
	`OptNum` smallint(5) NOT NULL COMMENT '变化个数(+：表示获得；-：表示失去)',

	`Serial1` bigint(20) NOT NULL COMMENT '物品64位序号',
	`ConType1` tinyint(1) NOT NULL COMMENT '容器编号(0表示没有在容器中)',
	`ResNum1` smallint(5) NOT NULL COMMENT '最终个数',
	
	`Serial2` bigint(20) NOT NULL COMMENT '物品64位序号(0表示没有)',
	`ConType2` tinyint(1) NOT NULL COMMENT '容器编号(0表示没有在容器中)',
	`ResNum2` smallint(5) NOT NULL COMMENT '最终个数',
	
	`RoleIDRel` int(10) unsigned NOT NULL default '4294967295' COMMENT '相关联角色ID',

	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '物品记录';
	
	
CREATE TABLE `log_itemtimes` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
		
	`RoleID` int(10) unsigned NOT NULL COMMENT 'role id',
	`CmdID` int(10) unsigned NOT NULL COMMENT '操作',

	`Serial` bigint(20) NOT NULL COMMENT '物品64位序号',
	`TypeID` int(10) unsigned NOT NULL COMMENT 'type id',
	`ConType` tinyint(1) NOT NULL COMMENT '容器编号(0表示没有在容器中)',
	`UsedTimes` int(10) NOT NULL COMMENT '已使用次数',
	`MaxUseTimes` int(10) NOT NULL COMMENT '最多可使用次数',

	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '物品记录';
	
	
CREATE TABLE `log_mallsell` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	
	`CmdID` int(10) unsigned NOT NULL COMMENT '操作',
	`Serial` bigint(20) NOT NULL COMMENT '64为序号',
	`TypeID` int(10) unsigned NOT NULL COMMENT '物品id',
	`RoleID` int(10) unsigned NOT NULL COMMENT '购买者id',
	`Num` smallint(5) NOT NULL COMMENT '个数',
	`FstGainTime` char(20) NOT NULL default '2000-00-00 00:00:00' COMMENT '第一个玩家获得该道具的时间',
	`ExistTime` int(10) unsigned NOT NULL COMMENT '时限(单位：秒)',
	`MaxUseTimes` int(10) NOT NULL COMMENT '最多可使用次数',
	`CostYB` int(10) NOT NULL COMMENT '购买价格(总价：元宝)',
	`CostExVolume` int(10) NOT NULL COMMENT '兑换花费的赠点数量',
	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '商城物品出售记录';
	
CREATE TABLE `log_fund` (                                      
            `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',     
            `account_id` int(10) unsigned NOT NULL COMMENT '账号ID',   
            `role_id` int(10) unsigned NOT NULL COMMENT '角色ID',      
            `cmd_id` int(10) NOT NULL COMMENT '操作',                  
            `fund` int(10) NOT NULL COMMENT '帮派资金变化',        
            `total_fund` int(10) NOT NULL COMMENT '帮派当前资金',  
            `time` time NOT NULL COMMENT 'log时间'                     
          ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `log_material` (                                      
                `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',         
                `account_id` int(10) unsigned NOT NULL COMMENT '账号ID',       
                `role_id` int(10) unsigned NOT NULL COMMENT '角色ID',          
                `cmd_id` int(10) NOT NULL COMMENT '操作',                      
                `material` int(10) NOT NULL COMMENT '帮派资材变化',        
                `total_material` int(10) NOT NULL COMMENT '帮派当前资材',  
                `time` time NOT NULL COMMENT 'log时间'                         
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `log_reputation` (                                      
                  `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',           
                  `account_id` int(10) unsigned NOT NULL COMMENT '账号ID',         
                  `role_id` int(10) unsigned NOT NULL COMMENT '角色ID',            
                  `cmd_id` int(10) NOT NULL COMMENT '操作',                        
                  `reputation` int(10) NOT NULL COMMENT '帮派声望变化',        
                  `total_reputation` int(10) NOT NULL COMMENT '帮派当前声望',  
                  `time` time NOT NULL COMMENT 'log时间'                           
                ) ENGINE=MyISAM DEFAULT CHARSET=utf8;	


CREATE TABLE `log_mallsell_pack` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',

	`PackID` int(10) unsigned NOT NULL COMMENT '礼品包id',
	`BuyID` int(10) unsigned NOT NULL COMMENT '购买者role id',
	`ToID` int(10) unsigned NOT NULL COMMENT '获得者role id',
	`CostYB` int(10) NOT NULL COMMENT '购买价格(总价：元宝)',

	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '商城出售的礼品包记录';

CREATE TABLE `log_GMCmd` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',

	`role_id` int(10) unsigned NOT NULL COMMENT '角色id',
	`gm_cmd` varchar(256) NOT NULL COMMENT 'Gm命令',
	`error_code` int(10) unsigned NOT NULL COMMENT '执行命令的返回码',
	`time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '角色使用GM命令的log';


CREATE TABLE `log_Pet` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	`master_id` int(10) unsigned NOT NULL COMMENT '角色id',
	`pet_id` int(10) unsigned NOT NULL COMMENT '宠物id',
	`cmd_id` int(10) unsigned NOT NULL COMMENT '操作',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '角色对宠物操作的log';
