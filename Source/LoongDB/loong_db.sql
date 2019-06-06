CREATE TABLE `account_common` (
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',
	`AccountName` char(36) NOT NULL COMMENT '账号名',
	`SafeCodeCrc` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '安全码Crc32',
	`ResetTime` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '重置安全码',
	`BagPsdCrc` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '行囊密码CRC值',
	`BaiBaoYuanBao` int(10) NOT NULL DEFAULT '0' COMMENT '百宝袋中元宝',
	`WareSize` smallint(6) NOT NULL DEFAULT '24' COMMENT '仓库大小,默认值为24',
	`WareSilver` bigint(20) NOT NULL DEFAULT '0' COMMENT '仓库中金钱数',
	PRIMARY KEY (`AccountID`),
	KEY `AccountName` (`AccountName`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='游戏世界内账号相关属性';

CREATE TABLE `roledata` (
	`AccountID` int(10) unsigned NOT NULL,
	
	`RoleID` int(10) unsigned NOT NULL,
	`RoleName` varchar(32) NOT NULL,
	`RoleNameCrc` int(10) unsigned NOT NULL,

	`Sex` tinyint(3) NOT NULL COMMENT '性别(0.女;1.男)',

	`HairModelID` tinyint(3) unsigned NOT NULL COMMENT '发型',
	`HairColorID` tinyint(3) unsigned NOT NULL COMMENT '发色',
	`FaceModelID` tinyint(3) unsigned NOT NULL COMMENT '面部五官',
	`FaceDetailID` tinyint(3) unsigned NOT NULL COMMENT '面部细节',
	`DressModelID` tinyint(3) unsigned NOT NULL COMMENT '服装样式',
	
	`AvatarEquip` tinyblob COMMENT '装备外观',
	`DisplaySet` tinyint(3) NOT NULL default '1' COMMENT '外观显示设置(默认显示时装模式)',

	`MapID` int(10) unsigned NOT NULL COMMENT '角色所在地图ID',
	`X` float(10) NOT NULL COMMENT '角色所在地图中的X坐标',
	`Y` float(10) NOT NULL COMMENT '角色所在地图中的Y坐标',
	`Z` float(10) NOT NULL COMMENT '角色所在地图中的Z坐标',
	
	`FaceX` float(10) NOT NULL COMMENT '角色朝向的X坐标',
	`FaceY` float(10) NOT NULL COMMENT '角色朝向的Y坐标',
	`FaceZ` float(10) NOT NULL COMMENT '角色朝向的Z坐标',
	
	`RebornMapID` int(10) unsigned NOT NULL default '4294967295' COMMENT '角色复活地图ID',
	
	`Class` tinyint(3) NOT NULL default '1' COMMENT '专精职业',
	`ClassEx` tinyint(3) NOT NULL default '0' COMMENT '英雄职业',

	`Level` smallint(3) NOT NULL default '1' COMMENT '当前等级',
	`ExpCurLevel` int(10) unsigned NOT NULL default '0' COMMENT '当前级别的升级经验',
	
	`HP` mediumint(8) NOT NULL default '-1' COMMENT '血量',
	`MP` mediumint(8) NOT NULL default '-1' COMMENT '真气量',
	`Rage` smallint(6) NOT NULL default '0' COMMENT '怒气',
	`Endurance` smallint(6) NOT NULL default '-1' COMMENT '持久力',
	`Vitality` smallint(5) NOT NULL default '-1' COMMENT '活力(0～999)',
	`Injury` mediumint(8) NOT NULL default '0' COMMENT '内伤',
	`Knowledge` mediumint(8) NOT NULL default '0' COMMENT '阅历',
	`Morale` smallint(4) NOT NULL default '100' COMMENT '士气',
	`Morality` mediumint(8) NOT NULL default '0' COMMENT '道德',
	`Culture` mediumint(8) NOT NULL default '0' COMMENT '武学修为',
	`Credit` mediumint(8) NOT NULL default '0' COMMENT '信用度',
	`Identity` tinyint(3) NOT NULL default '0' COMMENT '身份',
	`VIPPoint` mediumint(8) NOT NULL default '0' COMMENT '会员积分',
	
	`AttPtAvail` smallint(6) NOT NULL default '0' COMMENT '可投属性点',
	`TalentPtAvail` smallint(6) NOT NULL default '0' COMMENT '可投天资点数',
	
	`PhysiqueAdded` smallint(6) NOT NULL default '0' COMMENT '筋骨',
	`StrengthAdded` smallint(6) NOT NULL default '0' COMMENT '劲力',
	`PneumaAdded` smallint(6) NOT NULL default '0' COMMENT '元气',
	`InnerforceAdded` smallint(6) NOT NULL default '0' COMMENT '内力',
	`TechniqueAdded` smallint(6) NOT NULL default '0' COMMENT '技力',
	`AgilityAdded` smallint(6) NOT NULL default '0' COMMENT '身法',
	
	`Strength` mediumint(8) NOT NULL default '0' COMMENT '实力值',
	`StrengthNum` smallint(6) NOT NULL default '0' COMMENT '实力排名',
	`Heroism` mediumint(8) NOT NULL default '0' COMMENT '英雄值',
	`Wit` mediumint(8) NOT NULL default '0' COMMENT '谋略值',
	`Errantry` mediumint(8) NOT NULL default '0' COMMENT '侠义值',
	`Valor` mediumint(8) NOT NULL default '0' COMMENT '政战值',
	
	`TalentType1` tinyint(2) NOT NULL default '-1' COMMENT '天资类型',
	`TalentType2` tinyint(2) NOT NULL default '-1' COMMENT '天资类型',
	`TalentType3` tinyint(2) NOT NULL default '-1' COMMENT '天资类型',
	`TalentType4` tinyint(2) NOT NULL default '-1' COMMENT '天资类型',
	
	`TalentVal1` smallint(4) NOT NULL default '0' COMMENT '天资投放值',
	`TalentVal2` smallint(4) NOT NULL default '0' COMMENT '天资投放值',
	`TalentVal3` smallint(4) NOT NULL default '0' COMMENT '天资投放值',
	`TalentVal4` smallint(4) NOT NULL default '0' COMMENT '天资投放值',
	
	`SGFlag` tinyint(1) NOT NULL default '1' COMMENT '是否被PK保护',
	`CloseSGTime` char(20) NOT NULL default '2000-01-01 00:00:00' COMMENT '上次关闭PK保护的时间',
	
	`BagSize` smallint(6) NOT NULL default '20' COMMENT '背包大小,默认为20',
	`BagGold` int(10) NOT NULL default '0' COMMENT '背包中金子数',
	`BagSilver` int(10) NOT NULL default '0' COMMENT '背包中蝇子数',
	`BagYuanBao` int(10) NOT NULL default '0' COMMENT '背包中元宝数',
	`ExchangeVolume` int(10) NOT NULL default '0' COMMENT '玩家角色赠点的数量',
	
	`GuildID` int(10) unsigned NOT NULL default '4294967295' COMMENT '玩家所在帮派id',
	`TotalTax` int(10) NOT NULL default '0' COMMENT '缴税总额',
	
	`RemoteOpenSet` int(10) unsigned NOT NULL default '4294967295' COMMENT '是否对远端玩家公开相关信息',
	`CurTitleID` smallint(6) unsigned NOT NULL default '65535' COMMENT '当前称号',
	
	`GetMallFreeTime` char(20) NOT NULL default '2000-00-00 00:00:00' COMMENT '上次从商城领取免费物品时间',
	
	`CreateTime` char(20) NOT NULL COMMENT '创建时间',
	`LoginTime` char(20) NOT NULL default '2000-00-00 00:00:00' COMMENT '上线时间',
	`LogoutTime` char(20) NOT NULL default '2000-00-00 00:00:00' COMMENT '下线时间',
	`OnlineTime` int(10) NOT NULL default '-1' COMMENT '总在线时间(单位:秒)',
	`CurOnlineTime` int(10) NOT NULL default '0' COMMENT '当前在线时间(单位:秒)',
	
	`RemoveFlag` tinyint(1) NOT NULL default '0' COMMENT '是否处于删除状态(该状态玩家不会被读入游戏世界)',
	`RemoveTime` int(10) unsigned NOT NULL default '4294967295' COMMENT '删除时间',
	`ScriptData` blob COMMENT '脚本数据',
	`TreasureSum` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '角色开启宝箱数',

        `StallLevel` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT '摊位等级',                                                                                                                                                                                                    
        `StallDailyExp` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '摊位当天经验',                                                                                                                                                                                              
        `StallCurExp` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '摊位当前等级经验', 
	`StallLastTime` int(10) unsigned NOT NULL DEFAULT '4325376' COMMENT '摊位最近更新时间(默认2000年1月1日)',
	`Hostility` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '戾气值',
	`DeadUnSetSafeGuardCountDown` smallint(5) signed NOT NULL DEFAULT '-100' COMMENT '关闭玩家戾气为0时回城复活所加上的pk保护倒计tick',
	`ItemTransportMapID` int(10) unsigned NOT NULL DEFAULT '3017298127' COMMENT '记录式传送符mapid',
	`ItemTransportX` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符X坐标 2200*50',
	`ItemTransportZ` float unsigned NOT NULL DEFAULT '110000' COMMENT '记录式传送符Z坐标 2200*50',
	`ItemTransportY` float unsigned NOT NULL DEFAULT '332300' COMMENT '记录式传送符Y坐标 6646*50',
	
	`LoverID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '爱人对应的roleid没结婚就是4294967295',
	`HaveWedding` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否举行过婚礼',
	`LastLessingTime` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '上次领取神龙赐福的时间',
	`LastLessingLevel` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '领取到了多少级', 
	`NeedPrisonRevive` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '是否需要牢狱复活', 
	PRIMARY KEY (`RoleID`),
	KEY `AccountID` (`AccountID`),
	UNIQUE KEY `RoleNameCrc` (`RoleNameCrc`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色数据表 -- 核心表';
	

CREATE TABLE `skill` (                     
	`RoleID` int(10) unsigned NOT NULL,
	`ID` int(10) unsigned NOT NULL,
	`BiddenLevel` tinyint(3) NOT NULL default '0' COMMENT '投点等级',
	`SelfLevel` tinyint(3) NOT NULL default '0' COMMENT '固有等级',
	`Proficiency` mediumint(8) NOT NULL default '0' COMMENT '技能熟练程度',
	`CoolDown` int(10) NOT NULL COMMENT 'buff作用时间(单位:毫秒)',
	PRIMARY KEY  (`RoleID`, `ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色所会的功夫';


CREATE TABLE `buff` (                     
	`RoleID` int(10) unsigned NOT NULL COMMENT '在谁身上',
	`SrcUnitID` int(10) unsigned NOT NULL COMMENT '源Unit的ID',
	`SrcSkillID` int(10) unsigned NOT NULL COMMENT '由哪个技能产生（大ID）',
	`ItemTypeID` int(10) unsigned NOT NULL COMMENT '物品或装备的属性ID',
	`ItemSerialID` bigint(10) NOT NULL COMMENT '由哪个物品或装备产生',
	
	`BuffID` int(10) unsigned NOT NULL COMMENT 'Buff ID',
	`CurTick` int(10) unsigned NOT NULL COMMENT '当前持续时间(单位:tick)',
	`Level` tinyint(3) NOT NULL COMMENT '等级',
	`CurLapTimes` tinyint(3) NOT NULL COMMENT '当前叠加次数',
	
	`EffectSkillID` blob COMMENT '影响该buff的技能ID',

	PRIMARY KEY  (`RoleID`, `BuffID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色身上的buff';


CREATE TABLE `title` (                     
	`RoleID` int(10) unsigned NOT NULL COMMENT '角色ID',
	`ID` smallint(5) unsigned NOT NULL COMMENT '称号ID',
	`Count` int(10) unsigned NOT NULL COMMENT '计数',
	PRIMARY KEY  (`RoleID`, `ID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色称号表';


CREATE TABLE `item` (                                                                                                 
          `SerialNum` bigint(20) NOT NULL COMMENT '64位id',                                                                  
          `Num` smallint(5) NOT NULL DEFAULT '1' COMMENT '数量',                                                            
          `TypeID` int(10) unsigned NOT NULL COMMENT '类型id',                                                              
          `Bind` tinyint(4) NOT NULL COMMENT '是否绑定',                                                                  
          `LockStat` tinyint(4) NOT NULL DEFAULT '0' COMMENT '物品锁定状态(1:锁定状态)',                            
          `UseTimes` int(10) NOT NULL DEFAULT '0' COMMENT '物品使用次数',                                               
          `FirstGainTime` char(20) NOT NULL DEFAULT '2000-00-00 00:00:00' COMMENT '第一个玩家获得该道具的时间',  
          `CreateMode` tinyint(2) NOT NULL COMMENT '创建方式：GM，NPC，任务和玩家等',                            
          `CreateID` int(10) unsigned NOT NULL COMMENT '创建方式中对应的id',                                          
          `CreatorID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '创建者id',                                   
          `CreateTime` char(20) NOT NULL COMMENT '创建时间',                                                              
          `OwnerID` int(10) unsigned NOT NULL COMMENT '角色ID',                                                             
          `AccountID` int(10) unsigned NOT NULL COMMENT '帐号ID(各角色共享物品用)',                                 
          `ContainerTypeID` tinyint(4) unsigned NOT NULL COMMENT '所在容器类型',                                        
          `Suffix` tinyint(4) unsigned NOT NULL COMMENT '容器中位置',                                                    
          `NameID` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '名字ID',                                         
          PRIMARY KEY (`SerialNum`),                                                                                          
          KEY `OwnerID` (`OwnerID`),                                                                                          
          KEY `AccountID` (`AccountID`,`ContainerTypeID`)                                                                     
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='物品表';                                                      

	
CREATE TABLE `equip` (
	`SerialNum` bigint(20) NOT NULL,

	# 装备鉴定时确定
	`Quality` tinyint(4) unsigned NOT NULL default '0' COMMENT '品级(255表示未鉴定)',

	`MinUseLevel` tinyint(4) unsigned NOT NULL default '0' COMMENT '等级限制',
	`MaxUseLevel` tinyint(4) unsigned NOT NULL default '0' COMMENT '等级上限',

	`WuHun` smallint(5) NOT NULL COMMENT '武魂:内功伤害计算用',
	`MinDmg` smallint(5) NOT NULL COMMENT '武器最小伤害',
	`MaxDmg` smallint(5) NOT NULL COMMENT '武器最大伤害',
	`Armor` smallint(5) NOT NULL COMMENT '防具护甲',

	`PotVal` int(10) NOT NULL COMMENT '当前装备潜力值',
	`PotValUsed` int(10) NOT NULL default '0' COMMENT '已经消耗的装备潜力值',
	# 装备潜力值提升次数
	`PotIncTimes` smallint(10) UNSIGNED DEFAULT '0' NOT NULL,
	# 紫色品级鉴定几率 2009-12-2 Jason
	`PurpleQltyIdfPct` int(10) UNSIGNED DEFAULT '0' NOT NULL,

	`RoleAttEffect` tinyblob COMMENT '一级属性影响',
	
	`LongInnerID` int(10) unsigned NOT NULL default '4294967295' COMMENT '表龙魂能力ID',
	`LongOuterID` int(10) unsigned NOT NULL default '4294967295' COMMENT '里龙魂能力ID',

	`SpecAtt` tinyint(2) unsigned NOT NULL default '255' COMMENT '特殊属性',

	# 时装生成/鉴定时确定
	`Appearance` smallint(5) NOT NULL default '0' COMMENT '仪容',
	`Rein` tinyint(3) unsigned NOT NULL default '0' COMMENT '统御',
	`Savvy` tinyint(3) unsigned NOT NULL default '0' COMMENT '悟性',
	`Fortune` tinyint(3) unsigned NOT NULL default '0' COMMENT '福缘',
	`ColorID` tinyint(3) NOT NULL default '0' COMMENT '颜色代码',
	
	# 生产时影响的属性
	`AttALimMod` smallint(5) NOT NULL default '0' COMMENT '一级属性限制值',
	`AttALimModPct` smallint(5) NOT NULL default '0' COMMENT '一级属性限制百分比',
	
	`PosyTimes` tinyint(4) NOT NULL default '0' COMMENT '铭文次数',
	`PosyEffect` tinyblob COMMENT '铭文影响(体力,真气,内外攻防)',
	
	`EngraveTimes` tinyint(4) NOT NULL default '0' COMMENT '镌刻次数',
	`EngraveAtt` tinyblob COMMENT '镌刻影响(武器:攻击/武魂;防具:护甲)',
	
	`HoleNum` tinyint(4) NOT NULL default '0' COMMENT '镶嵌孔个数',
	`HoleGemID` tinyblob COMMENT '5个孔中镶嵌的宝石ID',
	
	`BrandLevel` tinyint(4) NOT NULL default '0' COMMENT '烙印等级',
	`DerateVal` tinyblob COMMENT '伤害减免值(若人物属性枚举中伤害减免顺序调整,则该处也需调整)',
	
	`LongFuLevel` tinyint(4) NOT NULL default '0' COMMENT '龙附等级',
	`WuXing` tinyblob COMMENT '五行等级: 金木水火土',
	
	`CanCut` tinyint(2) NOT NULL default '1' COMMENT '是否可开凿(0:否; 1:可)',

	`FlareVal` tinyint(3) unsigned NOT NULL default '0' COMMENT '光晕值',

	# 装备生产时确定
	`QltyModPct` smallint(10) NOT NULL default '0' COMMENT '一级生成品级修正率(-10000,10000)',
	`QltyModPctEx` smallint(10) unsigned NOT NULL default '0' COMMENT '二级生成品级修正率[0,10000)',
	`PotValModPct` smallint(10) unsigned NOT NULL DEFAULT '10000' COMMENT '装备潜力值影响系数',

	PRIMARY KEY  (`SerialNum`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='装备表(装备 = 装备表 + 物品表)';
	
	
CREATE TABLE `item_cdtime` (                                                        
               `RoleID` int(10) unsigned NOT NULL COMMENT '角色id',                            
               `CDTime` blob COMMENT '冷却时间blob，格式为（typeid，冷却时间）',  
               PRIMARY KEY (`RoleID`)                                                            
             ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='物品&装备冷却时间' ;
	
CREATE TABLE `friend` (                                              
          `RoleID` int(10) unsigned NOT NULL COMMENT '玩家id',             
          `FriendID` int(10) unsigned NOT NULL COMMENT '好友id',           
          `GroupID` tinyint(2) NOT NULL COMMENT '好友所属群组',        
          PRIMARY KEY (`RoleID`,`FriendID`)                                  
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='好友列表(单向)';
	
	
CREATE TABLE `friendship` (               
	`RoleID` int(10) unsigned NOT NULL COMMENT '好友中id较小的',  
	`FriendID` int(10) unsigned NOT NULL COMMENT '好友中id较大的',
	`FriVal` mediumint(6) NOT NULL COMMENT '友好度',
	PRIMARY KEY  (`RoleID`, `FriendID`),
	KEY `FriendID` (`FriendID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='友好度(双向)';
	
	
CREATE TABLE `enemy` (                                     
          `RoleID` int(10) unsigned NOT NULL COMMENT '玩家id',   
          `EnemyID` int(10) unsigned NOT NULL COMMENT '仇敌id',  
          PRIMARY KEY (`RoleID`,`EnemyID`)                         
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='仇敌';
	
CREATE TABLE `blacklist` (                                                        
             `RoleID` int(10) unsigned NOT NULL COMMENT '角色id',                          
             `BlackID` int(10) unsigned NOT NULL COMMENT '被角色id列入黑名单的id',  
             PRIMARY KEY (`RoleID`,`BlackID`)                                                
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='黑名单';

	
CREATE TABLE `task_done` (                                                                           
             `RoleID` int(10) unsigned NOT NULL,                                                                
             `TaskID` int(10) unsigned NOT NULL COMMENT '任务id',                                             
             `Times` smallint(6) unsigned NOT NULL COMMENT '一个时期内完成的任务次数',              
             `StartTime` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '第一个任务开始时间',  
             PRIMARY KEY (`RoleID`,`TaskID`)                                                                    
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='已做过任务';

	
	
CREATE TABLE `task` (                                                                 
          `RoleID` int(10) unsigned NOT NULL,                                                 
          `TaskID` int(10) unsigned NOT NULL,                                                 
          `TaskFlag` int(10) NOT NULL DEFAULT '0' COMMENT '任务完成状态',               
          `Monster0Num` int(10) NOT NULL DEFAULT '0' COMMENT '已打怪数量1',              
          `Monster1Num` int(10) NOT NULL DEFAULT '0',                                         
          `Monster2Num` int(10) NOT NULL DEFAULT '0',                                         
          `Monster3Num` int(10) NOT NULL DEFAULT '0',                                         
          `Monster4Num` int(10) NOT NULL DEFAULT '0',                                         
          `Monster5Num` int(10) NOT NULL DEFAULT '0',                                         
          `StartTime` int(10) unsigned NOT NULL DEFAULT '4294967295' COMMENT '开始时间',  
          `ScriptData` blob COMMENT '脚本数据',                                           
          `DynamicTarget` blob,                                                               
          PRIMARY KEY (`RoleID`,`TaskID`)                                                     
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='已接任务';

CREATE TABLE `role_del` (  
	`AccountID` int(10) unsigned NOT NULL,
	`RoleID` int(10) unsigned NOT NULL,
	`RoleName` varchar(32) NOT NULL,
	`IP` varchar(32),   
	`DeleteTime` datetime NOT NULL
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色删除相关记录'; 
	
	
CREATE TABLE `ybaccount` (
	`RoleID` int(10) unsigned NOT NULL,
	`YuanBao` int(10) NOT NULL default '0',
	`Gold` int(10) NOT NULL default '0',
	`Silver` int(10) NOT NULL default '0',
	`bSellOrder` tinyint(2) NOT NULL default '0' COMMENT '是否提交过出售订单',
	`bBuyOrder` tinyint(2) NOT NULL default '0' COMMENT ' 是否提交过收购订单',
	PRIMARY KEY (`RoleID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='元宝交易账户';
	
	
CREATE TABLE `yuanbaoorder` (
	`dwID` int(10) unsigned NOT NULL COMMENT '订单ID',
	`RoleID` int(10) unsigned NOT NULL COMMENT '玩家ID',
	`OrderType` tinyint(6) NOT NULL default '0' COMMENT '订单类型（收购出售）',
	`Price` int(10) NOT NULL default '0' COMMENT '订单中元宝价格',
	`Number` int(10) NOT NULL default '0' COMMENT '订单中元宝数量',
	`OrderMode` int(6) NOT NULL default '0' COMMENT '订单交易状态（委托中撤销关闭）',
	`StartTime` int(10) unsigned NOT NULL default '0' COMMENT '订单的提交时间',
	`EndTime` int(10) unsigned NOT NULL default '4294967295' COMMENT '订单的结束时间',
	`AvgPrice` int(10) NOT NULL default '0' COMMENT '最终成交的平均价格',
	`DealNum` int (10) NOT NULL default '0' COMMENT '最终的成交数量',
	PRIMARY KEY (`dwID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='元宝交易订单';

CREATE TABLE `visiting_card` (                                                 
                 `RoleID` int(11) unsigned NOT NULL COMMENT '角色id',                       
                 `Level` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT '等级',                    
                 `Job` tinyint(3) NOT NULL DEFAULT '0' COMMENT '职业',                      
                 `MateRoleID` int(11) NOT NULL DEFAULT '-1' COMMENT '配偶',                 
                 `FactionID` int(11) NOT NULL DEFAULT '-1' COMMENT '帮派',                  
                 `Position` int(3) NOT NULL DEFAULT '-1' COMMENT '帮派职位',              
                 `Visibility` tinyint(1) NOT NULL DEFAULT '1' COMMENT '可见性',            
                 `Sex` tinyint(2) unsigned NOT NULL DEFAULT '3' COMMENT '性别',             
                 `Constellation` tinyint(4) unsigned NOT NULL DEFAULT '13' COMMENT '星座',  
                 `ChineseZodiac` tinyint(4) unsigned NOT NULL DEFAULT '13' COMMENT '生肖',  
                 `Area` tinyint(4) unsigned NOT NULL DEFAULT '0' COMMENT '区域',            
                 `Province` tinyint(6) unsigned NOT NULL DEFAULT '0' COMMENT '省份',        
                 `City` char(6) NOT NULL DEFAULT 'N/A' COMMENT '城市',                     
                 `HeadUrl` varchar(100) DEFAULT NULL COMMENT '头像',              
                 `Signature` varchar(200) NOT NULL DEFAULT 'N/A' COMMENT '签名',            
                 PRIMARY KEY (`RoleID`)                                                       
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='角色名贴表';
               
CREATE TABLE `clan_data` (                                                                       
             `RoleID` int(11) unsigned NOT NULL,                                                            
             `FameMask` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '名人堂标识',                    
             `RepRstTimeStamp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '声望重置时间',          
             `RepXuanYuan` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '轩辕声望',                    
             `ConXuanYuan` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '轩辕贡献',                    
             `ActCntXuanYuan` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '轩辕剩余激活次数',  
             `RepShenNong` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '神农声望',                    
             `ConShenNong` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '神农贡献',                    
             `ActCntShenNong` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '神农剩余激活次数',  
             `RepFuXi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '伏羲声望',                        
             `ConFuXi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '伏羲贡献',                        
             `ActCntFuXi` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '伏羲剩余激活次数',      
             `RepSanMiao` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '三苗声望',                     
             `ConSanMiao` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '三苗贡献',                     
             `ActCntSanMiao` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '三苗剩余激活次数',   
             `RepJiuLi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '九黎声望',                       
             `ConJiuLi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '九黎贡献',                       
             `ActCntJiuLi` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '九黎剩余激活次数',     
             `RepYueZhi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '月氏声望',                      
             `ConYueZhi` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '月氏贡献',                      
             `ActCntYueZhi` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '月氏剩余激活次数',    
             `RepNvWa` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '女娲声望',                        
             `ConNvWa` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '女娲贡献',                        
             `ActCntNvWa` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '女娲剩余激活次数',      
             `RepGongGong` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '共工声望',                    
             `ConGongGong` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '共工贡献',                    
             `ActCntGongGong` tinyint(2) unsigned NOT NULL DEFAULT '0' COMMENT '共工剩余激活次数',  
             PRIMARY KEY (`RoleID`),                                                                        
             KEY `RepFuXi` (`RepFuXi`),                                                                     
             KEY `RepXuanYuan` (`RepXuanYuan`),                                                             
             KEY `RepSanMiao` (`RepSanMiao`),                                                               
             KEY `RepShenNong` (`RepShenNong`),                                                             
             KEY `RepJiuLi` (`RepJiuLi`),                                                                   
             KEY `RepGongGong` (`RepGongGong`),                                                             
             KEY `RepNvWa` (`RepNvWa`),                                                                     
             KEY `RepYueZhi` (`RepYueZhi`)                                                                  
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='玩家氏族数据表';                                          

CREATE TABLE `famehall_act_treasure` (                               
	 `TreasureID` smallint(11) unsigned NOT NULL COMMENT '氏族珍宝ID',  
	 `ActNameID` int(11) unsigned NOT NULL COMMENT '激活该珍宝角色名字ID',                             
	 `RoleID` int(11) unsigned NOT NULL COMMENT '角色ID',                                
	 `ClanType` tinyint(1) unsigned NOT NULL COMMENT '氏族类型',                           
	 `ActTime` int(11) unsigned NOT NULL COMMENT '激活时间',                               
	 PRIMARY KEY (`ClanType`,`TreasureID`),                              
	 KEY `ActTime` (`ActTime`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='氏族珍宝激活表';

CREATE TABLE `famehall_entersnap` (         
	`RoleID` int(11) unsigned NOT NULL COMMENT '角色ID',       
	`EnterNameID` int(11) unsigned NOT NULL COMMENT '角色名字ID',  
	`ClanType` tinyint(3) unsigned NOT NULL COMMENT '氏族类型',  
	`EnterTime` int(11) unsigned NOT NULL COMMENT '进入时间',    
	PRIMARY KEY (`ClanType`,`EnterTime`),     
	KEY `ClanType` (`ClanType`)               
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;


CREATE TABLE `famehall_rep_rst_timestamp` (    
	`ClanType` tinyint(8) NOT NULL COMMENT '氏族类型',              
	`ResetTimeStamp` int(11) unsigned NOT NULL COMMENT '重置时间',  
	PRIMARY KEY (`ClanType`)                     
) ENGINE=MyISAM DEFAULT CHARSET=utf8;   
	

CREATE TABLE `guild` (                         
	`ID` int(10) unsigned NOT NULL COMMENT '帮派ID -- 名称转成小写后的CRC32值',
	`Name` blob COMMENT '名称',
	
	`FounderNameID` int(10) unsigned NOT NULL COMMENT '帮派创始人名称ID',
	`LeaderID` int(10) unsigned NOT NULL COMMENT '现任帮主ID',
	`SpecState` int(10) unsigned NOT NULL default '0' COMMENT '帮派当前特殊状态',
	
	`Level` tinyint(3) NOT NULL COMMENT '帮派当前等级',
	`HoldCity0` tinyint(3) unsigned NOT NULL default '0' COMMENT '帮派当前辖属城市编号',
	`HoldCity1` tinyint(3) unsigned NOT NULL default '0' COMMENT '',
	`HoldCity2` tinyint(3) unsigned NOT NULL default '0' COMMENT '',
	
	`Fund` int(10) NOT NULL default '0' COMMENT '当前资金',
	`Material` int(10) NOT NULL default '0' COMMENT '当前资材',
	`Rep` int(10) NOT NULL default '0' COMMENT '当前威望',
	`DailyCost` int(10) NOT NULL default '0' COMMENT '每日维护费用',
	
	`Peace` smallint(5) NOT NULL default '0' COMMENT '当前安宁度',
	`Rank` smallint(5) NOT NULL default '0' COMMENT '当前排名(0表示还未排名)',
	
	`Tenet` blob COMMENT '宗旨',
	`Symbol` blob COMMENT '标志ULR',
	
	`CreateTime` datetime NOT NULL COMMENT '创建时间',
	`GroupPurchase` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '帮派团购指数', 
	`RemainSpreadTimes` int(3) unsigned NOT NULL DEFAULT '0' COMMENT '帮务剩余发布次数',                                                           
    `Commendation` tinyint(1) NOT NULL DEFAULT '0' COMMENT '跑商嘉奖状态',
	PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='帮派属性';
	

CREATE TABLE `guild_member` (
	`RoleID` int(10) unsigned NOT NULL COMMENT '成员ID',
	`GuildID` int(10) unsigned NOT NULL COMMENT '帮派ID',
	`GuildPos` tinyint(3) unsigned NOT NULL COMMENT '帮派职位',
	
	`TotalContrib` int(10) NOT NULL default '0' COMMENT '累计帮派贡献',
	`CurContrib` int(10) NOT NULL default '0' COMMENT '当前帮派贡献',
	`Exploit` int(10) NOT NULL default '0' COMMENT '帮派功勋',
	`Salary` int(10) NOT NULL default '0' COMMENT '俸禄标准',
	`CanUseGuildWare` tinyint(1) NOT NULL DEFAULT '0' COMMENT '操作帮派仓库权限',
	`JoinTime` datetime NOT NULL COMMENT '加入时间',
	PRIMARY KEY (`RoleID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='帮派成员';
	

CREATE TABLE `log_baibao` (
	`AccountID` int(10) unsigned NOT NULL COMMENT '账号ID',         
	`RoleID` int(10) NOT NULL COMMENT '物品来源',                   
	`LogTime` char(20) NOT NULL COMMENT '获得物品时间',             
	`n16Type` tinyint(4) unsigned NOT NULL COMMENT '物品获得方式',  
	`TypeID` int(10) unsigned NOT NULL COMMENT '物品类型',          
	`LeaveWords` tinyblob COMMENT '赠言'                            
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
            
CREATE TABLE `group_purchase` (                                       
	`GuildID` int(10) unsigned NOT NULL COMMENT '帮派ID',               
	`RoleID` int(10) NOT NULL COMMENT '发起人ID',                       
	`MallID` int(10) NOT NULL COMMENT '商城物品ID',                     
	`ItemPrice` int(10) unsigned NOT NULL COMMENT '商品价格',           
	`ParticipatorNum` tinyint(3) unsigned NOT NULL COMMENT '响应人数',  
	`RequireNum` tinyint(3) unsigned NOT NULL COMMENT '团购所需人数',   
	`RemainTime` int(10) NOT NULL COMMENT '团购剩余时间',               
	`ParticipatorList` tinyblob COMMENT '响应者列表'                    
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
               
CREATE TABLE `pet_data` (                                                                                  
            `pet_id` int(11) unsigned NOT NULL COMMENT '宠物id',
            `pet_name` varchar(32) NOT NULL COMMENT  '宠物名称',
            `master_id` int(11) unsigned NOT NULL COMMENT '主人id',
            `type_id` int(11) unsigned NOT NULL COMMENT '原型id',
            `quality` tinyint(1) unsigned NOT NULL COMMENT '宠物品质',
            `aptitude` int(4) unsigned NOT NULL COMMENT '宠物资质',
            `potential` int(4) unsigned NOT NULL COMMENT '当前潜能',
            `cur_spirit` int(4) NOT NULL COMMENT '当前灵力',
            `cur_exp` int(20) NOT NULL DEFAULT '0' COMMENT '当前经验',
            `step` tinyint(1) NOT NULL DEFAULT '0' COMMENT '阶',
            `grade` tinyint(1) NOT NULL DEFAULT '1' COMMENT '等',
            `talent_count` int(4) NOT NULL DEFAULT '0' COMMENT '当前天资计数',
            `wuxing_energy` int(4) NOT NULL DEFAULT '0' COMMENT '五行力',
            `pet_state` int(1) NOT NULL DEFAULT '0'COMMENT '宠物状态',
            `pet_lock` tinyint(1) NOT NULL DEFAULT '0' COMMENT '宠物锁定',
            PRIMARY KEY (`pet_id`)
          ) ENGINE=MyISAM DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='宠物表';

CREATE TABLE `vip_stall` (                                       
             `StallID` tinyint(2) unsigned NOT NULL COMMENT 'VIP摊位序号',  
             `RoleID` int(10) unsigned NOT NULL COMMENT '所有者ID',         
             `RemainTime` int(10) NOT NULL COMMENT '剩余时间',              
             PRIMARY KEY (`StallID`)                                        
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `pet_skill` (                                               
             `petskill_typeid` int(11) unsigned NOT NULL COMMENT '宠物技能id',  
             `petid` int(11) unsigned NOT NULL COMMENT '宠物id',                  
             `para1` int(11) unsigned DEFAULT NULL COMMENT '参数1',      
             `para2` int(11) unsigned DEFAULT NULL COMMENT '参数2',      
             PRIMARY KEY (`petskill_typeid`,`petid`)                                
             KEY `petid` (`petid`)                                              
           ) ENGINE=MyISAM DEFAULT CHARSET=utf8;                               

CREATE TABLE `guild_commodity` (                                                                 
                   `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                                        
                   `guild_id` int(10) unsigned NOT NULL COMMENT '所在帮派ID',                                 
                   `role_level` smallint(3) unsigned NOT NULL DEFAULT '1' COMMENT '开始跑商时玩家等级',  
                   `tael` int(10) NOT NULL DEFAULT '0' COMMENT '当前商银数量',                              
                   `goods` tinyblob COMMENT '商货信息',                                                       
                   PRIMARY KEY (`role_id`)                                                                        
                 ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
     
CREATE TABLE `guild_skill` (                                         
               `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',           
               `skill_id` int(10) unsigned NOT NULL COMMENT '技能ID',           
               `progress` int(10) unsigned NOT NULL COMMENT '升级进度',       
               `level` int(2) unsigned NOT NULL COMMENT '当前等级',           
               `researching` tinyint(1) NOT NULL COMMENT '当前正在研究中'  
             ) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `vip_netbar` (                                                                                  
              `accountid` int(11) unsigned NOT NULL COMMENT '账号id',                                                  
              `login_time` int(11) unsigned NOT NULL COMMENT '上次在金牌网吧登录时间',                        
              PRIMARY KEY (`accountid`)                                                                                  
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8 CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC COMMENT='金牌网吧';


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
               
CREATE TABLE `commerce_rank` (                                             
                 `role_id` int(10) unsigned NOT NULL COMMENT '玩家ID',                  
                 `guild_id` int(10) unsigned NOT NULL COMMENT '帮派ID',                 
                 `times` int(10) NOT NULL DEFAULT '0' COMMENT '完成跑商次数',       
                 `tael` int(10) NOT NULL DEFAULT '0' COMMENT '为帮派贡献的商银',  
                 PRIMARY KEY (`role_id`)                                                  
               ) ENGINE=MyISAM DEFAULT CHARSET=utf8;    
               
create table `activity` (   
		 `dwID` int (10)   NOT NULL ,  
		 `ScriptData` blob   NULL  , 
		 PRIMARY KEY ( `dwID` )  
		) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='固定活动数据';           

CREATE TABLE `left_msg` (                                                                                    
            `msg_id` int(11) unsigned NOT NULL COMMENT '留言日期时间',                                           
            `roleid` int(11) unsigned NOT NULL COMMENT '玩家id',                                                     
            `msg_data` tinyblob COMMENT '留言内容',                                                                
            PRIMARY KEY (`msg_id`,`roleid`)                                                                            
          ) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='留言数据';

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

# 创建实力排行记录表
CREATE TABLE `strength_rankings` (                     
	`Num` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '名次',
	`PreNum` smallint(6) unsigned NOT NULL DEFAULT '0' COMMENT '上次名次',
	`Strength` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT '实力值',
	`RoleName` varchar(32) NOT NULL COMMENT '角色名字', 
	PRIMARY KEY  (`RoleName`)
	);


#数据库增加索引
#ALTER TABLE `item_del` ADD KEY (`SerialNum`);
ALTER TABLE `yuanbaoorder` ADD KEY (`OrderMode`);
ALTER TABLE `yuanbaoorder` ADD KEY (`RoleID`);
ALTER TABLE `guild_upgrade` ADD KEY (`guild_id`);	
ALTER TABLE `guild_skill` ADD KEY (`guild_id`);
ALTER TABLE `guild_commodity` ADD KEY (`guild_id`);
ALTER TABLE `commerce_rank` ADD KEY (`guild_id`);
ALTER TABLE `roledata` ADD KEY (`RemoveFlag`);
#ALTER TABLE `item_needlog` ADD KEY (`NeedLog`);
ALTER TABLE `group_purchase` ADD KEY (`GuildID`);
ALTER TABLE `group_purchase` ADD KEY (`RoleID`);
ALTER TABLE `group_purchase` ADD KEY (`MallID`);
ALTER TABLE `pet_data` ADD KEY (`master_id`);
ALTER TABLE `yuanbaoorder` ADD KEY (`StartTime`);
#ALTER TABLE `account` ADD KEY (`worldname_crc`);
ALTER TABLE `roledata` ADD KEY (`rolename`);
ALTER TABLE `account_common` ADD KEY (`AccountName`);
ALTER TABLE `task_done` ADD KEY (`RoleID`);
  
  
# Jason 外部链接表
CREATE TABLE `external_links` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `LinkName` varchar(10) NOT NULL DEFAULT '"NoName"' COMMENT '??????',
  `LinkURL` varchar(256) DEFAULT NULL COMMENT '????URL',
  PRIMARY KEY (`ID`),
  KEY `LinkName` (`LinkName`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;
  

################## 所有新表在此之上加 #######################

	
CREATE TABLE `item_needlog` (               
	`TypeID` int(10) unsigned NOT NULL COMMENT '物品的TypeID(唯一)',
	`NeedLog` tinyint(2) unsigned NOT NULL default '1' COMMENT '是否记录log(0:不记录;1:记录)',
	`MinQlty` tinyint(2) NOT NULL default '0' COMMENT '需记录log的物品的最低品级',
	PRIMARY KEY  (`TypeID`)
	) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='需做log的物品TypeID表，由运营填写'; 
	

#复制物品和装备表结构
CREATE TABLE `item_baibao` SELECT * FROM `item` WHERE 0;
ALTER TABLE `item_baibao` ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='物品表';
ALTER TABLE `item_baibao` ADD PRIMARY KEY  (`SerialNum`);
ALTER TABLE `item_baibao` ADD KEY (`AccountID`);

CREATE TABLE `equip_baibao` LIKE `equip`;

CREATE TABLE `item_del` LIKE `item`;
Alter table `item_del` drop primary key;

CREATE TABLE `equip_del` LIKE `equip`;
Alter table `equip_del` drop primary key;

CREATE TABLE `roledata_del` LIKE `roledata`;
ALTER TABLE `roledata_del` DROP PRIMARY KEY;
ALTER TABLE `roledata_del` DROP INDEX `RoleNameCrc`;


