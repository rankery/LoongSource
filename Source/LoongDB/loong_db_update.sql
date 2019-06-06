############################## 1.1.25 start #########################################

##2009-06-23
#account_common中添加AccountName字段
alter table `account_common` add column `AccountName` char (36)   NOT NULL  COMMENT '账号名' after `AccountID`;

############################## 1.1.25 end ###########################################

############################## 1.2.0 start ###########################################

##2009-07-06
#增加物品划拨表
CREATE TABLE `bill_item` (                 
             `token_id` char(50) NOT NULL COMMENT '令牌ID',          
             `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',  
             `item_id` int(10) unsigned NOT NULL COMMENT '物品ID',     
             `item_num` int(11) NOT NULL COMMENT '物品数量',             
             PRIMARY KEY (`token_id`)                 
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;       

##2009-07-06
#增加元宝划拨表
CREATE TABLE `bill_yuanbao` (                       
                `token_id` char(50) NOT NULL COMMENT '令牌ID',  
                `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',          
                `yuanbao` int(11) NOT NULL COMMENT '元宝数量',                       
                PRIMARY KEY (`token_id`)                          
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
              
##2009-07-07
#增加宠物数据表
CREATE TABLE `pet_data` (                                                                                       
	`pet_id` int(11) unsigned NOT NULL COMMENT '宠物id',                                                        
	`pet_name` varchar(32) NOT NULL COMMENT '宠物名称',                                                       
	`master_id` int(11) unsigned NOT NULL COMMENT '主人id',                                                     
	`type_id` int(11) unsigned NOT NULL COMMENT '原型id',                                                       
	`quality` tinyint(1) unsigned NOT NULL COMMENT '宠物品质',                                                
	`aptitude` tinyint(3) unsigned NOT NULL COMMENT '宠物资质',                                               
	`cur_spirit` int(4) NOT NULL COMMENT '当前灵力',                                                          
	`mountable` tinyint(1) NOT NULL COMMENT '能否骑乘',                                                       
	`cur_exp` int(20) NOT NULL DEFAULT '0' COMMENT '当前经验',                                                
	`step` tinyint(1) NOT NULL DEFAULT '0' COMMENT '阶',                                                         
	`grade` tinyint(1) NOT NULL DEFAULT '1' COMMENT '等',                                                        
	`talent_count` tinyint(4) NOT NULL DEFAULT '0' COMMENT '当前天资计数',                                  
	`wuxing_energy` tinyint(4) NOT NULL DEFAULT '0' COMMENT '五行力',                                          
	`pet_state` tinyint(1) NOT NULL DEFAULT '0' COMMENT '宠物状态',                                           
	PRIMARY KEY (`pet_id`)                                                                                        
) ENGINE=MyISAM DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='宠物表' ;

##2009-07-08
#增加VIP摊位表
CREATE TABLE `vip_stall` (                                       
             `StallID` tinyint(2) unsigned NOT NULL COMMENT 'VIP摊位序号',  
             `RoleID` int(10) unsigned NOT NULL COMMENT '所有者ID',         
             `RemainTime` int(10) NOT NULL COMMENT '剩余时间',              
             PRIMARY KEY (`StallID`)                                        
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

##2009-07-10
#宠物数据表增加潜能字段
alter table `pet_data` add column `potential` smallint(4) UNSIGNED NOT NULL COMMENT '当前潜能' after `aptitude`;

#2009-7-15 roledata表增加脚本数据blob
alter table roledata add column `ScriptData` blob NULL COMMENT '脚本数据' after `RemoveTime`;

##2009-7-17
#为装备增加潜力值影响系数
alter table `equip` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;
alter table `equip_baibao` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;
alter table `equip_del` add column `PotValModPct` smallint(10) UNSIGNED DEFAULT '10000' NOT NULL COMMENT '装备潜力值影响系数' after `QltyModPctEx`;

##2009-7-21
#增加宠物技能表格
create table `pet_skill`( 
	`petskill_typeid` int(11) NOT NULL COMMENT '宠物技能id', 
	`petid` int(11) NOT NULL COMMENT '宠物id', 
	`para1` int(11) ZEROFILL COMMENT '参数1', 
	`para2` int(11) ZEROFILL COMMENT '参数2', 
	PRIMARY KEY (`petskill_typeid`, `petid`)
	)  ;

##2009-7-22
#修改宠物数据表格
alter table `pet_data` drop column `mountable`;
						

############################## 1.2.0 end ############################################


############################## 1.2.1 start ##########################################
##2009-7-23
#修改角色名贴表
alter table `visiting_card` change `Level` `Level` tinyint(3) UNSIGNED default '1' NOT NULL comment '等级';

##2009-7-28
#修改宠物技能表
alter table `pet_skill` change `petskill_typeid` `petskill_typeid` int(11) UNSIGNED NOT NULL comment '宠物技能id', 
						change `petid` `petid` int(11) UNSIGNED NOT NULL comment '宠物id',
						change `para1` `para1` int(11) UNSIGNED NULL  comment '参数1', 
						change `para2` `para2` int(11) UNSIGNED NULL  comment '参数2';

#2009-7-29 roledata表增加角色开启宝箱计数
alter table roledata add column `TreasureSum` tinyint(3) unsigned default '0' NOT NULL COMMENT '角色开启宝箱数' after `ScriptData`;

##2009-08-03
#修改仓库保存金钱格式
alter table `account_common` drop column `WareGold`;
alter table `account_common` change `WareSilver` `WareSilver` int(20) default '0' NOT NULL;

#追加帮派属性字段
alter table `guild` add column `LeaderID` int(10) UNSIGNED NOT NULL COMMENT '帮主ID' after `FounderNameID`;
alter table `guild` add column `RemainSpreadTimes` int(3) DEFAULT '0' NOT NULL COMMENT '帮务发布剩余次数';
alter table `guild` add column `Commendation` bool DEFAULT '0' NOT NULL COMMENT '跑商嘉奖状态';

#追加帮派成员属性
alter table `guild_member` add column `CanUseGuildWare` bool DEFAULT '0' NOT NULL COMMENT '操作帮派仓库权限';

#创建帮派跑商容器表
CREATE TABLE `guild_commodity` (                                                                 
                   `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                                        
                   `guild_id` int(10) unsigned NOT NULL COMMENT '所在帮派ID',                                 
                   `role_level` smallint(3) unsigned NOT NULL DEFAULT '1' COMMENT '开始跑商时玩家等级',  
                   `tael` int(10) NOT NULL DEFAULT '0' COMMENT '当前商银数量',                              
                   `goods` tinyblob COMMENT '商货信息',                                                       
                   PRIMARY KEY (`role_id`)                                                                        
                 ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#创建帮派技能表
CREATE TABLE `guild_skill` (                                         
               `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',           
               `skill_id` int(10) unsigned NOT NULL COMMENT '技能ID',           
               `progress` int(10) unsigned NOT NULL COMMENT '升级进度',       
               `level` int(2) unsigned NOT NULL COMMENT '当前等级',           
               `researching` tinyint(1) NOT NULL COMMENT '当前正在研究中'  
             ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

#创建帮派设施升级表
CREATE TABLE `guild_upgrade` (                                           
                 `guild_id` int(10) unsigned NOT NULL COMMENT '设施所属帮派',     
                 `type` int(2) unsigned NOT NULL COMMENT '帮派设施类型',          
                 `level` int(2) unsigned NOT NULL COMMENT '设施等级',               
                 `progress` int(10) unsigned NOT NULL COMMENT '升级进度',           
                 `item_type_1` int(10) unsigned NOT NULL COMMENT '需求物品类型',  
                 `item_neednum_1` int(10) unsigned NOT NULL COMMENT '需求的数量',  
                 `item_type_2` int(10) unsigned NOT NULL,                               
                 `item_neednum_2` int(10) unsigned NOT NULL,                            
                 `item_type_3` int(10) unsigned NOT NULL,                               
                 `item_neednum_3` int(10) unsigned NOT NULL,                            
                 `item_type_4` int(10) unsigned NOT NULL,                               
                 `item_neednum_4` int(10) unsigned NOT NULL                             
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
               
#创建帮派跑商排行榜表
CREATE TABLE `commerce_rank` (                                             
                 `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                  
                 `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',                 
                 `times` int(10) NOT NULL DEFAULT '0' COMMENT '完成跑商次数',       
                 `tael` int(10) NOT NULL DEFAULT '0' COMMENT '为帮派贡献的商银',  
                 PRIMARY KEY (`role_id`)                                                  
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
               
						
						alter table `blacklist` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '角色id', 
						change `BlackID` `BlackID` int(10) UNSIGNED NOT NULL comment '被角色id列入黑名单的id';


##2009-8-5
#增加数据库表注释
alter table `clan_data` change `RepXuanYuan` `RepXuanYuan` int(11) UNSIGNED default '0' NOT NULL comment '轩辕声望', 
						change `ConXuanYuan` `ConXuanYuan` int(11) UNSIGNED default '0' NOT NULL comment '轩辕贡献', 
						change `ActCntXuanYuan` `ActCntXuanYuan` tinyint(2) UNSIGNED default '0' NOT NULL comment '轩辕剩余激活次数', 
						change `RepShenNong` `RepShenNong` int(11) UNSIGNED default '0' NOT NULL comment '神农声望', 
						change `ConShenNong` `ConShenNong` int(11) UNSIGNED default '0' NOT NULL comment '神农贡献', 
						change `ActCntShenNong` `ActCntShenNong` tinyint(2) UNSIGNED default '0' NOT NULL comment '神农剩余激活次数', 
						change `RepFuXi` `RepFuXi` int(11) UNSIGNED default '0' NOT NULL comment '伏羲声望', 
						change `ConFuXi` `ConFuXi` int(11) UNSIGNED default '0' NOT NULL comment '伏羲贡献', 
						change `ActCntFuXi` `ActCntFuXi` tinyint(2) UNSIGNED default '0' NOT NULL comment '伏羲剩余激活次数', 
						change `RepSanMiao` `RepSanMiao` int(11) UNSIGNED default '0' NOT NULL comment '三苗声望', 
						change `ConSanMiao` `ConSanMiao` int(11) UNSIGNED default '0' NOT NULL comment '三苗贡献', 
						change `ActCntSanMiao` `ActCntSanMiao` tinyint(2) UNSIGNED default '0' NOT NULL comment '三苗剩余激活次数', 
						change `RepJiuLi` `RepJiuLi` int(11) UNSIGNED default '0' NOT NULL comment '九黎声望', 
						change `ConJiuLi` `ConJiuLi` int(11) UNSIGNED default '0' NOT NULL comment '九黎贡献', 
						change `ActCntJiuLi` `ActCntJiuLi` tinyint(2) UNSIGNED default '0' NOT NULL comment '九黎剩余激活次数', 
						change `RepYueZhi` `RepYueZhi` int(11) UNSIGNED default '0' NOT NULL comment '月氏声望', 
						change `ConYueZhi` `ConYueZhi` int(11) UNSIGNED default '0' NOT NULL comment '月氏贡献', 
						change `ActCntYueZhi` `ActCntYueZhi` tinyint(2) UNSIGNED default '0' NOT NULL comment '月氏剩余激活次数', 
						change `RepNvWa` `RepNvWa` int(11) UNSIGNED default '0' NOT NULL comment '女娲声望', 
						change `ConNvWa` `ConNvWa` int(11) UNSIGNED default '0' NOT NULL comment '女娲贡献', 
						change `ActCntNvWa` `ActCntNvWa` tinyint(2) UNSIGNED default '0' NOT NULL comment '女娲剩余激活次数', 
						change `RepGongGong` `RepGongGong` int(11) UNSIGNED default '0' NOT NULL comment '共工声望', 
						change `ConGongGong` `ConGongGong` int(11) UNSIGNED default '0' NOT NULL comment '共工贡献', 
						change `ActCntGongGong` `ActCntGongGong` tinyint(2) UNSIGNED default '0' NOT NULL comment '共工剩余激活次数';
						
alter table `enemy` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '玩家id', 
					change `EnemyID` `EnemyID` int(10) UNSIGNED NOT NULL comment '仇敌id';

alter table `friend` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '玩家id', 
					change `FriendID` `FriendID` int(10) UNSIGNED NOT NULL comment '好友id', 
					change `GroupID` `GroupID` tinyint(2) NOT NULL comment '好友所属群组';
					
alter table `item` change `SerialNum` `SerialNum` bigint(20) NOT NULL comment '64位id', 
					change `Num` `Num` smallint(5) default '1' NOT NULL comment '数量', 
					change `TypeID` `TypeID` int(10) UNSIGNED NOT NULL comment '类型id', 
					change `Bind` `Bind` tinyint(4) NOT NULL comment '是否绑定', 
					change `CreatorID` `CreatorID` int(10) UNSIGNED default '4294967295' NOT NULL comment '创建者id';
					
alter table `item_cdtime` change `RoleID` `RoleID` int(10) UNSIGNED NOT NULL comment '角色id', 
					change `CDTime` `CDTime` blob NULL  comment '冷却时间blob，格式为（typeid，冷却时间）';
					
alter table `task` change `Monster0Num` `Monster0Num` int(10) default '0' NOT NULL comment '已打怪数量1', 
					change `StartTime` `StartTime` int(10) UNSIGNED default '4294967295' NOT NULL comment '开始时间', 
					change `ScriptData` `ScriptData` blob NULL  comment '脚本数据', 
					change `DynamicTarget` `DynamicTarget` blob NULL ;
					
alter table `task_done` change `TaskID` `TaskID` int(10) UNSIGNED NOT NULL comment '任务id', 
					change `Times` `Times` smallint(6) UNSIGNED NOT NULL comment '一个时期内完成的任务次数', 
					change `StartTime` `StartTime` int(10) UNSIGNED default '4294967295' NOT NULL comment '第一个任务开始时间';

alter table `visiting_card` change `City` `City` char(6) character set utf8 collate utf8_general_ci default 'N/A' NOT NULL comment '城市';

##2009-8-5
#增加金牌网吧数据库表
CREATE TABLE `vip_netbar` (                                                                                  
              `accountid` int(11) unsigned NOT NULL COMMENT '账号id',                                                  
              `login_time` int(11) unsigned NOT NULL COMMENT '上次在金牌网吧登录时间',                        
              PRIMARY KEY (`accountid`)                                                                                  
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='金牌网吧'  ;

##2009-8-11
#固定活动脚本数据
create table `activity` (    `dwID` int (10)   NOT NULL ,  
			`ScriptData` blob   NULL  , 
			PRIMARY KEY ( `dwID` )  
			) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='固定活动数据';      

##2009-8-20
#修改宠物表
alter table `pet_data` change `potential` `potential` int(4) UNSIGNED NOT NULL comment '当前潜能';

##2009-9-23
#修改帮派默认值
alter table `guild` change `HoldCity0` `HoldCity0` tinyint(3) UNSIGNED default '0' NOT NULL comment '帮派当前辖属城市编号';
alter table `guild` change `HoldCity1` `HoldCity1` tinyint(3) UNSIGNED default '0' NOT NULL;
alter table `guild` change `HoldCity2` `HoldCity2` tinyint(3) UNSIGNED default '0' NOT NULL;
update guild set HoldCity0=0,HoldCity1=0,HoldCity2=0;

############################## 1.2.1 end ############################################

############################## 1.2.2 start ##########################################


#增加角色摊位成长所需字段
alter table `roledata` add column `StallLevel` tinyint(3) UNSIGNED DEFAULT '1' NOT NULL COMMENT '摊位等级';
alter table `roledata` add column `StallDailyExp` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '摊位当天经验';
alter table `roledata` add column `StallCurExp` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '摊位当前等级经验';
alter table `roledata` add column `StallLastTime` int(10) UNSIGNED DEFAULT '4325376' NOT NULL COMMENT '摊位最近更新时间(默认2000年1月1日)';

##2009-09-11
#添加宠物锁定字段
alter table `pet_data` add column `pet_lock` bool DEFAULT '0' NOT NULL COMMENT '宠物锁定' after `pet_state`;


##2009-09-16
#留言表
CREATE TABLE `left_msg` (                                                                                    
            `msg_id` int(11) unsigned NOT NULL COMMENT '留言日期时间',                                           
            `roleid` int(11) unsigned NOT NULL COMMENT '玩家id',                                                     
            `msg_data` tinyblob COMMENT '留言内容',                                                                
            PRIMARY KEY (`msg_id`,`roleid`)                                                                            
          ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='留言数据' ;
          
##2009-09-21
#删除equip_del主键
Alter table `equip_del` drop primary key;

##2009-09-28
#修改角色职业和英雄职业的默认值为0
alter table `roledata` drop column `Class`; 
alter table `roledata` add column `Class` tinyint (3) DEFAULT '1' NOT NULL after `RebornMapID`;
alter table `roledata` drop column `ClassEx`; 
alter table `roledata` add column `ClassEx` tinyint (3) DEFAULT '0' NOT NULL after `Class`;

##2009-10-14
#修改角色职业和英雄职业的默认值为0
alter table `visiting_card` change `HeadUrl` `HeadUrl` varchar(100) character set utf8 collate utf8_general_ci NULL  comment '头像url';

#2009-10-14
# rebuild table 'item_del'
drop table `item_del`;
CREATE TABLE `item_del` LIKE `item`;
Alter table `item_del` drop primary key;

#2009-10-19
# 增加一个新的天资字段
alter table `roledata` add column `TalentType4` tinyint(2) NOT NULL default '-1' COMMENT '天资类型' after `TalentType3`;
alter table `roledata` add column `TalentVal4` smallint(4) NOT NULL default '0' COMMENT '天资投放值' after `TalentVal3`;

#2009-10-19
# 增加元宝划拨日志表
CREATE TABLE `bill_yuanbao_log` (
  `token_id` char(50) NOT NULL DEFAULT '' COMMENT '令牌ID',
  `account_id` int(11) unsigned DEFAULT NULL,
  `yuanbao` int(11) DEFAULT NULL,
  `time` char(50) DEFAULT NULL,
  PRIMARY KEY (`token_id`),
  UNIQUE KEY `account_id` (`account_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='元宝划拨log';  

##2009-10-20
#修改角色名贴中
update visiting_card set HeadUrl=null where HeadUrl='N/A';

##2009-10-26
#修改宠物表中资质的范围
alter table `pet_data` change `aptitude` `aptitude` int(4) UNSIGNED NOT NULL comment '宠物资质';

##2009-10-29
#修改索引属性去掉unique限制
alter table `bill_yuanbao_log` drop key `account_id`, add key `account_id` ( `account_id` );

##2009-10-30
#增加item划拨log
CREATE TABLE `bill_item_log` (                 
 `token_id` char(50) NOT NULL COMMENT '令牌ID',          
 `account_id` int(10) unsigned NOT NULL COMMENT '帐号ID',  
 `item_id` int(10) unsigned NOT NULL COMMENT '物品ID',     
 `item_num` int(11) NOT NULL COMMENT '物品数量',
 `time` char(50) DEFAULT NULL,             
 PRIMARY KEY (`token_id`),
 KEY `account_id` (`account_id`)               
) ENGINE=MyISAM DEFAULT CHARSET=utf8; 

#删除划拨表(会清除之前的数据,慎用!!!)
drop table `bill_item_log`;
drop table `bill_yuanbao_log`;
drop table `bill_item`;
drop table `bill_yuanbao`;

#增加中央划拨表
CREATE TABLE `centralbilling` (                                    
                  `TOKENID` int(11) NOT NULL COMMENT '令牌ID',                   
                  `USERNAME` varchar(50) NOT NULL COMMENT '帐号',                
                  `WORLDID` int(11) DEFAULT NULL COMMENT '服ID',                  
                  `ZONEID` int(11) NOT NULL COMMENT '区ID',                       
                  `YUANORITEM` tinyint(1) NOT NULL COMMENT '0 元宝；1 道具',  
                  `ITEMCODE` int(11) DEFAULT NULL COMMENT '道具编号',          
                  `QTY` int(11) NOT NULL DEFAULT '0' COMMENT '数量',             
                  `STATE` int(11) NOT NULL DEFAULT '0' COMMENT '状态',           
                  `INSERTDATE` datetime NOT NULL COMMENT '插入时间',           
                  PRIMARY KEY (`TOKENID`)                                          
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;
CREATE TABLE `centralbilling_log` LIKE `centralbilling`;

############################## 1.2.2 end ############################################

############################## 1.2.3 start ##########################################
##2009-11-5
alter table `roledata` change `CloseSGTime` `CloseSGTime` char (20)  DEFAULT '2000-01-01 00:00:00' NOT NULL  COMMENT '上次关闭PK保护的时间'

############################## 1.2.3 end ############################################

############################## 1.2.4 start ##########################################

##2009-11-18
#修改了宠物表中几个字段的类型
alter table `pet_data` change `talent_count` `talent_count` int(4) default '0' NOT NULL, change `wuxing_energy` `wuxing_energy` int(4) default '0' NOT NULL, change `pet_state` `pet_state` int(1) default '0' NOT NULL;
#数据库增加索引
ALTER TABLE `item_del` ADD KEY (`SerialNum`);
ALTER TABLE `yuanbaoorder` ADD KEY (`OrderMode`);
ALTER TABLE `yuanbaoorder` ADD KEY (`RoleID`);
ALTER TABLE `guild_upgrade` ADD KEY (`guild_id`);	
ALTER TABLE `guild_skill` ADD KEY (`guild_id`);
ALTER TABLE `guild_commodity` ADD KEY (`guild_id`);
ALTER TABLE `commerce_rank` ADD KEY (`guild_id`);
ALTER TABLE `roledata` ADD KEY (`RemoveFlag`);
ALTER TABLE `item_needlog` ADD KEY (`NeedLog`);
ALTER TABLE `group_purchase` ADD KEY (`GuildID`);
ALTER TABLE `group_purchase` ADD KEY (`RoleID`);
ALTER TABLE `group_purchase` ADD KEY (`MallID`);
ALTER TABLE `pet_data` ADD KEY (`master_id`);
ALTER TABLE `yuanbaoorder` ADD KEY (`StartTime`);
ALTER TABLE `account` ADD KEY (`worldname_crc`);
ALTER TABLE `roledata` ADD KEY (`rolename`);
ALTER TABLE `account_common` ADD KEY (`AccountName`);
ALTER TABLE `task_done` ADD KEY (`RoleID`);

############################## 1.2.4 end ############################################
############################## 1.2.5 start ##########################################
#2009-11-23 roledata表增加角色戾气值
alter table roledata add column `Hostility` tinyint(1) unsigned default '0' NOT NULL COMMENT '戾气值' after `StallLastTime`;

#2009-11-26 roledata表增加关闭玩家戾气为0时回城复活所加上的pk保护倒计tick
alter table roledata add column `DeadUnSetSafeGuardCountDown` smallint(4) unsigned default '0' NOT NULL COMMENT '关闭玩家戾气为0时回城复活所加上的pk保护倒计tick' after `Hostility`;

#2009-11-28 roledata表增加记录式传送符
alter table roledata add column  `ItemTransportMapID` int(10) unsigned NOT NULL DEFAULT '3017298127' COMMENT '记录式传送符mapid' after `DeadUnSetSafeGuardCountDown`;
alter table roledata add column  `ItemTransportX` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符X坐标2200*50' after `ItemTransportMapID`;
alter table roledata add column  `ItemTransportZ` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符Z坐标2200*50' after `ItemTransportX`;
alter table roledata add column  `ItemTransportY` float unsigned NOT NULL DEFAULT '332300' COMMENT '记录式传送符Y坐标6646*50' after `ItemTransportZ`;

alter table roledata change `ItemTransportX` `ItemTransportX` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符X坐标2200*50';
alter table roledata change `ItemTransportZ` `ItemTransportZ` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符Z坐标2200*50';
alter table roledata change `ItemTransportY` `ItemTransportY` float unsigned NOT NULL DEFAULT '332300' COMMENT '记录式传送符Y坐标6646*50';

#2009-12-15 account_common 修改库银类型
alter table `account_common` change `WareSilver` `WareSilver` bigint(20) default '0' NOT NULL;

#2009-12-17 roledata表修改DeadUnSetSafeGuardCountDown字段
alter table roledata change `DeadUnSetSafeGuardCountDown` `DeadUnSetSafeGuardCountDown` smallint(5) signed NOT NULL default '-100' COMMENT '关闭玩家戾气为0时回城复活所加上的pk保护倒计tick';
############################## 1.2.5 end ############################################



############################## 1.3.0 begin ############################################
#2009-09-17
#添加夫妻字段
alter table `roledata` add column `LoverID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '爱人对应的roleid没结婚就是4294967295'after `ItemTransportY`;
alter table `roledata` add column `HaveWedding` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否举行过婚礼' after `LoverID`;

# 装备增加潜力值提升次数
alter table `equip` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
alter table `equip_baibao` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
alter table `equip_del` add column `PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备潜力值提升次数' after `PotValUsed`;
##2009-12-03
#roledata表中增加实力值和实力排名字段
alter table `roledata` add column `Strength` mediumint(8) DEFAULT '0' NOT NULL COMMENT '实力值' after `AgilityAdded`;
alter table `roledata` add column `StrengthNum` smallint(6) DEFAULT '0' NOT NULL COMMENT '实力排名' after `Strength`;
alter table `roledata` add column `Heroism` mediumint(8) DEFAULT '0' NOT NULL COMMENT '英雄值' after `StrengthNum`;
alter table `roledata` add column `Wit` mediumint(8) DEFAULT '0' NOT NULL COMMENT '谋略值' after `Heroism`;
alter table `roledata` add column `Errantry` mediumint(8) DEFAULT '0' NOT NULL COMMENT '侠义值' after `Wit`;
alter table `roledata` add column `Valor` mediumint(8) DEFAULT '0' NOT NULL COMMENT '政战值' after `Errantry`;

# 创建实力排行记录表
CREATE TABLE `strength_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Strength` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '实力值',
	`RoleName` varchar(32) NOT NULL COMMENT '角色名字', 
	PRIMARY KEY  (`RoleName`)
	);

# 装备紫色品级鉴定几率
alter table `equip` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
alter table `equip_baibao` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
alter table `equip_del` add column `PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL COMMENT '装备紫色品级鉴定几率' after `PotIncTimes`;
# 2009-12-06
# 清空宠物技能冷却时间
update pet_skill set para1=4294967295 where para1<>4294967295;

# Jason 2009-12-7 增加神龙赐福，相关字段在roledata表内。
alter table `roledata` add column `LastLessingTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上次领取神龙赐福的时间' after `HaveWedding`;
alter table `roledata` add column `LastLessingLevel` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '领取到了多少级' after `LastLessingTime`;

alter table `roledata` add column `NeedPrisonRevive` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否需要牢狱复活' after `LastLessingLevel`;

#
alter table `roledata` change `DeadUnSetSafeGuardCountDown` `DeadUnSetSafeGuardCountDown` smallint(5) default '-100' NOT NULL;

############################## 1.3.0 end   ############################################



############################## 1.3.1 begin ############################################
# Jason 外部链接表
/*Table structure for table `external_links` */

DROP TABLE IF EXISTS `external_links`;

CREATE TABLE `external_links` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `LinkName` varchar(10) NOT NULL DEFAULT '"NoName"' COMMENT '??????',
  `LinkURL` varchar(256) DEFAULT NULL COMMENT '????URL',
  PRIMARY KEY (`ID`),
  KEY `LinkName` (`LinkName`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;


# 在pet_skill表中增加索引
alter table `pet_skill` add index `petid` (`petid`);
alter table `pet_skill` engine = MyISAM;
############################## 1.3.1 end   ############################################



