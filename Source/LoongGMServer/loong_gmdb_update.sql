############################## 1.1.25 start #########################################

##2009-06-24
#修改权限表中字段名
alter table `privilege` change `pri_s` `pri_3` int (4)  DEFAULT '1' NOT NULL;

############################## 1.1.25 end ###########################################

############################## 1.1.26 start #########################################
##2009-06-25
#修改权限表中字段
alter table `privilege` 
	drop column `pri_0`,
	add column `pri_4` int (4)  DEFAULT '0' NOT NULL  after `pri_3`, 
	add column `pri_5` int (4)  DEFAULT '0' NOT NULL  after `pri_4`,
	add column `pri_6` int (4)  DEFAULT '0' NOT NULL  after `pri_5`, 
	add column `pri_7` int (4)  DEFAULT '0' NOT NULL  after `pri_6`,
	change `pri_1` `pri_1` int (4)  DEFAULT '0' NOT NULL , 
	change `pri_2` `pri_2` int (4)  DEFAULT '0' NOT NULL , 
	change `pri_3` `pri_3` int (4)  DEFAULT '0' NOT NULL ;

##2009-06-26
#添加运维客户端ip绑定表
CREATE TABLE `rt_iptable` (                   
	`name` char(32) NOT NULL COMMENT '账号名',  
	`ip` char(30) NOT NULL COMMENT '绑定ip',    
	PRIMARY KEY (`name`)                        
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

##2009-06-29
#添加GM动作Log表
CREATE TABLE `gm_log` (                                                                                                                                                                                                                                                                                                                                              
          `AccountID` int(10) unsigned NOT NULL COMMENT '操作人账号ID',                                                                                                                                                                                                                                                                                                 
          `Name` char(32) NOT NULL COMMENT '操作人账号名称',                                                                                                                                                                                                                                                                                                          
          `Time` datetime NOT NULL COMMENT 'Log记录时间',                                                                                                                                                                                                                                                                                                                
          `OperIP` char(32) NOT NULL COMMENT '操作人IP',                                                                                                                                                                                                                                                                                                                  
          `Action` enum('线下GM创建','线下GM修改','线下GM删除','角色恢复','属性修改','道具生成','道具删除','开放双倍','双倍删除','自动循环公告','自动循环公告删除','颜色公告','右下角公告','右下角公告删除','开启服务器','关闭服务器','强制关闭服务器','绑定IP') NOT NULL COMMENT '操作动作',  
          `Details` varchar(512) NOT NULL COMMENT '操作描述'                                                                                                                                                                                                                                                                                                             
        ) ENGINE=MyISAM DEFAULT CHARSET=utf8;


############################## 1.1.26 end ###########################################

############################## 1.2.0 start ###########################################

##2009-07-01
#追加绑定IP的权限
insert into `privilege` values ('NRTC_BindIP',0,0,0,0,0,0,1);

#修改权限表中字段
alter table `privilege` 
	add column `pri_8` int (4)  DEFAULT '0' NOT NULL  after `pri_7`;

############################## 1.2.0 end #############################################

############################## 1.2.1 start ###########################################

##2009-08-20
#增加两个消息
insert into `privilege` values ('NRTC_SystemCmd',0,0,0,0,0,0,1,0);
insert into `privilege` values ('NRTC_MaxNum',0,0,0,0,0,0,0,1);

#修改权限表中字段,增加9级账号
alter table `privilege` 
	add column `pri_9` int (4)  DEFAULT '0' NOT NULL  after `pri_8`;
	
#增加消息
insert into `privilege` values ('NRTC_UpdateMall',0,0,0,0,0,0,0,0,1);

############################## 1.2.1 end #############################################

############################## 1.2.2 start ###########################################

# log中的Action位增加一个事件
alter table `gm_log` change `Action` `Action` enum ('线下GM创建','线下GM修改','线下GM删除','角色恢复','属性修改','道具生成','道具删除','开放双倍','双倍删除','自动循环公告','自动循环公告删除','颜色公告','右下角公告','右下角公告删除','开启服务器','关闭服务器','强制关闭服务器','绑定IP','发送系统命令');

# 增加道具恢复的消息
insert into `privilege` values ('NGMTC_ItemResume',0,0,0,1,0,0,0,0,0);

# 增加两个消息
insert into `privilege` values ('NGMTC_AccountForbid',0,1,0,0,0,0,0,0,0);
insert into `privilege` values ('NGMTC_Blacklist',0,0,1,0,0,0,0,0,0);

############################## 1.2.2 end #############################################

############################## 1.2.3 start ###########################################

# 重新确认各消息的权限
truncate table privilege；

insert into `privilege` values 

('NGMTC_AccountForbid',0,1,1,1,1,0,0,0,0),
('NGMTC_AutoNotice',1,1,1,1,1,0,0,0,0),
('NGMTC_Blacklist',0,0,1,1,1,0,0,0,0),
('NGMTC_Double',0,1,1,1,1,0,0,0,0),
('NGMTC_GMCreate',0,0,0,0,1,0,1,0,0),
('NGMTC_GMDelete',0,0,0,0,1,0,1,0,0),
('NGMTC_GMModify',0,0,0,0,1,0,1,0,0),
('NGMTC_ItemCreate',0,0,0,1,1,0,0,0,0),
('NGMTC_ItemDelete',0,0,1,1,1,0,0,0,0),
('NGMTC_ItemResume',0,0,0,1,1,0,0,0,0),
('NGMTC_ProModify',0,0,0,1,1,0,0,0,0),
('NGMTC_RightNotice',1,1,1,1,1,0,0,0,0),
('NGMTC_RoleResume',0,0,1,1,1,0,0,0,0),
('NRTC_BindIP',0,0,0,0,0,0,1,0,0),
('NRTC_Challenge',1,1,1,1,1,1,1,1,1),
('NRTC_Close',0,0,0,0,0,1,1,0,0),
('NRTC_MaxNum',0,0,0,0,0,0,0,1,0),
('NRTC_Start',0,0,0,0,0,1,1,0,0),
('NRTC_SystemCmd',0,0,0,0,0,0,1,0,0),
('NRTC_UpdateMall',0,0,0,0,0,0,0,0,1);

# 增加设定，取消定时聊天频道公告消息，以及取消右下角公告消息
insert into `privilege` values ('NGMTC_AutoChatNotice',0,1,1,1,1,0,0,0,0);
insert into `privilege` values ('NGMTC_CancelAutoChatNotice',0,1,1,1,1,0,0,0,0);
insert into `privilege` values ('NGMTC_CancelRightNotice',1,1,1,1,1,0,0,0,0);

############################## 1.2.3 end #############################################

############################## 1.2.4 start ###########################################
# log中的Action位增加一个事件
alter table `gm_log` change `Action` `Action` enum ('线下GM创建','线下GM修改','线下GM删除','角色恢复','属性修改','道具生成','道具删除','开放双倍','双倍删除','自动循环公告','自动循环公告删除','右下角公告','右下角公告删除','聊天频道公告','取消聊天频道公告','开启服务器','关闭服务器','强制关闭服务器','绑定IP','发送系统命令');
alter table `gm_log` change `Action` `Action` enum ('create GM account','modify GM account','delete GM account','recover role','modify role attribute','create object','delete object','double','cancel double','auto notice','cancel auto notice','right notice','cancel right notice','auto chat notice','cancel auto chat notice','start server','close server','close server forced','IP bind','send system cmd');

############################## 1.2.4 end #############################################

############################## 1.2.5 start #############################################

insert into `privilege` values 

('NGMTS_GMSetFackNumer',0,0,0,0,0,0,0,1,0)

############################## 1.2.5 end #############################################