############################## 1.2.1 start ##########################################

##2009-07-28
#log_item, log_money
alter table `log_item` add `RoleIDRel` int(10) unsigned NOT NULL default '4294967295' COMMENT '相关联角色ID' after `ResNum2`;
alter table `log_money` add `RoleIDRel` int(10) unsigned NOT NULL default '4294967295' COMMENT '相关联角色ID' after `TotalNum`;

##2009-08-03
#log_fund, log_material, log_reputation
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

##2009-08-13
#创建表`log_mallsell_pack`
CREATE TABLE `log_mallsell_pack` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',

	`PackID` int(10) unsigned NOT NULL COMMENT '礼品包id',
	`BuyID` int(10) unsigned NOT NULL COMMENT '购买者role id',
	`ToID` int(10) unsigned NOT NULL COMMENT '获得者role id',
	`CostYB` int(10) NOT NULL COMMENT '购买价格(总价：元宝)',

	`Time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '商城出售的礼品包记录';

############################## 1.2.1 end ###########################################

############################## 1.2.3 start #########################################

CREATE TABLE `log_GMCmd` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',

	`role_id` int(10) unsigned NOT NULL COMMENT '角色id',
	`gm_cmd` varchar(256) NOT NULL COMMENT 'Gm命令',
	`error_code` int(10) unsigned NOT NULL COMMENT '执行命令的返回码',
	`time` datetime NOT NULL COMMENT '时间',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '角色使用GM命令的log';

############################## 1.2.3 end ###########################################
############################## 1.2.5 start #########################################
CREATE TABLE `log_Pet` (
	`ID` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '流水号',
	`master_id` int(10) unsigned NOT NULL COMMENT '角色id',
	`pet_id` int(10) unsigned NOT NULL COMMENT '宠物id',
	`cmd_id` int(10) unsigned NOT NULL COMMENT '操作',
	PRIMARY KEY  (`ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT '角色对宠物操作的log';
############################## 1.2.5 end ###########################################
