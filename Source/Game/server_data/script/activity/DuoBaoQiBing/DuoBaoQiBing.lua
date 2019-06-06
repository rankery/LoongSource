----------------------------------------------------------------------------------------
-------------------------------- 夺宝奇兵活动 ------------------------------------------
----------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------
-- 一些常量
--------------------------------------------------------------------------------------
BaoXiang_ID						= 4500028						-- 宝箱物品ID

BaoXiang_ID_M04						= 6000214						-- 炼铜宝箱宝箱物品ID
BaoXiang_ID_M07						= 6000215						-- 秘银宝箱宝箱物品ID
BaoXiang_ID_M107						= 6000216						-- 铂金宝箱宝箱物品ID

MAP_ID_M04					= 3017299663						--涿鹿m04
MAP_ID_M07					=	3017299407					--昆仑m07
MAP_ID_M107				=	2291720561					--100级修炼场

DuoBaoQiBing_Active_ID	= 155									-- 夺宝奇兵活动ID
ERS_HoldChest					= 16777216						-- 夺宝奇兵活动携宝模式0x1000000
HoldChest_BuffID				= 9438201						-- 夺宝奇兵活动携宝模式0x1000000
ERS_SafeArea						=	16								-- 0x0010,	// 安全区
ERS_Safeguard					= 1									-- 被保护
ES_Dead								=	0									-- 死亡

-- 在C++注册的脚本事件
ESRE_OnChaseHolderDie				= 48							-- 夺宝奇兵活动携带宝箱者被杀死时触发事件
ESRE_ForceLootChase						= 49							-- 宝箱强制掉落
ESIE_CanUse									=	0							-- 物品是否可用
ESIE_CanDelGroundItem					=	2							-- 掉在地上的物品是否可以删除
ESIE_PutDownItem							=	3							-- 丢弃物品
ESIE_CanPickUpItem						=	4							-- 判断是否可以捡起物品
ESIE_PickUpItem								=	5							-- 捡起物品时触发
ESIE_CanPutDown							= 6							-- 判断物品是否可以丢弃
ESIE_CanDelItemOnRoleInit			= 7							-- 角色初始化时候，检查背包中物品需要删除（0表示需要删除， 1表示不需要）
ESAE_OnTimer									=	1							-- 定时更新
ESAE_OnStart									=	2							-- 活动开始
ESAE_OnEnd									=	3							-- 活动结束
ESAE_OnTimerMin							=	4							-- 活动每分钟更新
ESAE_OnActOpenClose					= 9							-- 控制台开启或关闭活动

-- 角色行走状态枚举
EMS_Swim							= 6					-- 游泳
EMS_SwimStand					= 7					-- 游泳漂浮状态

-- 错误码
E_Loot_Swim_Can_Not_Pickup_BaoXiang = 11			-- 游泳时不能拾取宝箱
E_Loot_Role_Level_Limit = 12									-- 角色等级低于20级不能拾取宝箱
E_Loot_Role_SafeArea_Limit = 13								-- 角色在安全区内不能拾取宝箱
E_Loot_Role_Safeguard_Limit = 17							-- PK保护状态不能拾取宝箱

EWeekDay = {["Sunday"] = 1, ["Monday"] = 2, ["Tuesday"] = 3, ["Wednesday"] = 4, ["Thursday"] = 5, ["Friday"] = 6, ["Saturday"] = 7}

Need_Save_To_DB_Count = 14									-- 每个宝箱需要保存到数据库的数据个数

--------------------------------------------------------------------------------------
-- 脚本局部变量
--------------------------------------------------------------------------------------
Before_Activity_Broadcast = 0				-- 活动开始前半个小时公告的播放标记，0：未播放，1：已经播放
Activity_Data = {}
Activity_Data[BaoXiang_ID_M04 ]  = {Save_To_DB = 1,																				-- 数据是否要保存数据库
															 BaoXiang_SerialID_High = 0,																-- 宝箱物品序列号（高位）                            
															 BaoXiang_SerialID_Low	= 0,																-- 宝箱物品序列号（低位）                            
															 BaoXiang_GroupID_High = 0,															-- 宝箱地物ID（高位）                                  
															 BaoXiang_GroupID_Low = 0,																-- 宝箱地物ID（低位）                                  
															 BaoXiang_MapID=MAP_ID_M04,														-- 宝箱所在场景ID                                         
															 BaoXiang_X=0,																					-- 宝箱X坐标                                                 
															 BaoXiang_Z = 0,																					-- 宝箱Z坐标                                                 
															 BaoXiang_Holder = 0,																		-- 拿到宝箱的玩家的角色ID                           
															 Min_Counter = 0,																				-- 分钟计数器                                                
															 Active_Started = 0,																				-- 活动是否开始                                             
															 Active_End = 0,																					-- 活动是否结束                                             
															 BaoXiang_OpenRemainTime = BaoXiang_CDTime,							-- 宝箱可开启剩余时间（单位为秒）              
															 Tick_Count = 0,																					-- tick计时                                                    
															 BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval}			-- 重新刷新宝箱的倒计时（单位为秒 ）          

Activity_Data[BaoXiang_ID_M07 ]  = {Save_To_DB = 1,																				-- 数据是否要保存数据库
															 BaoXiang_SerialID_High = 0,																-- 宝箱物品序列号（高位）                            
															 BaoXiang_SerialID_Low	= 0,																-- 宝箱物品序列号（低位）                            
															 BaoXiang_GroupID_High = 0,															-- 宝箱地物ID（高位）                                  
															 BaoXiang_GroupID_Low = 0,																-- 宝箱地物ID（低位）                                  
															 BaoXiang_MapID=MAP_ID_M07,														-- 宝箱所在场景ID                                         
															 BaoXiang_X=0,																					-- 宝箱X坐标                                                 
															 BaoXiang_Z = 0,																					-- 宝箱Z坐标                                                 
															 BaoXiang_Holder = 0,																		-- 拿到宝箱的玩家的角色ID                           
															 Min_Counter = 0,																				-- 分钟计数器                                                
															 Active_Started = 0,																				-- 活动是否开始                                             
															 Active_End = 0,																					-- 活动是否结束                                             
															 BaoXiang_OpenRemainTime = BaoXiang_CDTime,							-- 宝箱可开启剩余时间（单位为秒）              
															 Tick_Count = 0,																					-- tick计时                                                    
															 BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval}			-- 重新刷新宝箱的倒计时（单位为秒 ）          

Activity_Data[BaoXiang_ID_M107] = {Save_To_DB = 1,																				-- 数据是否要保存数据库
															 BaoXiang_SerialID_High = 0,																-- 宝箱物品序列号（高位）                            
															 BaoXiang_SerialID_Low	= 0,																-- 宝箱物品序列号（低位）                            
															 BaoXiang_GroupID_High = 0,															-- 宝箱地物ID（高位）                                  
															 BaoXiang_GroupID_Low = 0,																-- 宝箱地物ID（低位）                                  
															 BaoXiang_MapID=MAP_ID_M107,													-- 宝箱所在场景ID                                         
															 BaoXiang_X=0,																					-- 宝箱X坐标                                                 
															 BaoXiang_Z = 0,																					-- 宝箱Z坐标                                                 
															 BaoXiang_Holder = 0,																		-- 拿到宝箱的玩家的角色ID                           
															 Min_Counter = 0,																				-- 分钟计数器                                                
															 Active_Started = 0,																				-- 活动是否开始                                             
															 Active_End = 0,																					-- 活动是否结束                                             
															 BaoXiang_OpenRemainTime = BaoXiang_CDTime,							-- 宝箱可开启剩余时间（单位为秒）              
															 Tick_Count = 0,																					-- tick计时                                                    
															 BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval}			-- 重新刷新宝箱的倒计时（单位为秒 ）          

WeekDay_OpenState = {0, 0, 0, 0, 0, 0, 0}	 -- 夺宝奇兵开放状态，从周日开始

--------------------------------------------------------------------------------------
-- 脚本局部工具函数
--------------------------------------------------------------------------------------
-- 保存活动数据到数据库
function SaveActivityDataToDB(BaoXiang_ID)
--不需要保存数据库了
--[[	if Activity_Data[BaoXiang_ID].Save_To_DB == 1 then
		local IndexFactor = 0
		if BaoXiang_ID == BaoXiang_ID_M04 then
			IndexFactor = 0
		elseif BaoXiang_ID == BaoXiang_ID_M07 then
			IndexFactor = 1
		elseif BaoXiang_ID == BaoXiang_ID_M107 then
			IndexFactor = 2
		else
			return
		end

		local ScriptDataStartIdx = IndexFactor*Need_Save_To_DB_Count
		act.SetActScriptData(DuoBaoQiBing_Active_ID, Need_Save_To_DB_Count,
						ScriptDataStartIdx			,Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High,															-- 宝箱物品序列号（高位）                  
						ScriptDataStartIdx+1		,Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low,															-- 宝箱物品序列号（低位）                  
						ScriptDataStartIdx+2		,Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High,															-- 宝箱地物ID（高位）                         
						ScriptDataStartIdx+3		,Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low,															-- 宝箱地物ID（低位）                         
						ScriptDataStartIdx+4		,Activity_Data[BaoXiang_ID].BaoXiang_X,																				-- 宝箱X坐标                                        
						ScriptDataStartIdx+	5		,Activity_Data[BaoXiang_ID].BaoXiang_Z,																				-- 宝箱Z坐标                                        
						ScriptDataStartIdx+	6		,Activity_Data[BaoXiang_ID].BaoXiang_Holder,																		-- 拿到宝箱的玩家的角色ID                  
						ScriptDataStartIdx+	7		,Activity_Data[BaoXiang_ID].Min_Counter,																				-- 分钟计数器                                       
						ScriptDataStartIdx+	8		,Activity_Data[BaoXiang_ID].Active_Started,																			-- 活动是否开始                                   
						ScriptDataStartIdx+	9		,Activity_Data[BaoXiang_ID].Active_End,																				-- 活动是否结束                                   
						ScriptDataStartIdx+	10	,Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime,													-- 宝箱可开启剩余时间（单位为秒）     
						ScriptDataStartIdx+	11	,Activity_Data[BaoXiang_ID].Tick_Count,																				-- tick计时                                           
						ScriptDataStartIdx+12	 	,Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime)												-- 重新刷新宝箱的倒计时（单位为秒 ）
		act.SaveActScriptData(DuoBaoQiBing_Active_ID)
		Activity_Data[BaoXiang_ID].Save_To_DB = 0	
	end	--]]
end

--从数据库读取活动数据
function LoadActivityData(BaoXiang_ID)
--不需要保存数据库了
--[[	local IndexFactor = 0
	if BaoXiang_ID == BaoXiang_ID_M04 then
		IndexFactor = 0
	elseif BaoXiang_ID == BaoXiang_ID_M07 then
		IndexFactor = 1
	elseif BaoXiang_ID == BaoXiang_ID_M107 then
		IndexFactor = 2
	else
		return
	end

	local ScriptDataStartIdx = IndexFactor*Need_Save_To_DB_Count	-- 开始索引

	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High,					-- 宝箱物品序列号（高位）                     
	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low,						-- 宝箱物品序列号（低位）                     
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High,					-- 宝箱地物ID（高位）                           
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low,					-- 宝箱地物ID（低位）                           
	Activity_Data[BaoXiang_ID].BaoXiang_X,										-- 宝箱X坐标                                          
	Activity_Data[BaoXiang_ID].BaoXiang_Z,										-- 宝箱Z坐标                                          
	Activity_Data[BaoXiang_ID].BaoXiang_Holder,								-- 拿到宝箱的玩家的角色ID                    
	Activity_Data[BaoXiang_ID].Min_Counter,										-- 分钟计数器                                         
	Activity_Data[BaoXiang_ID].Active_Started,									-- 活动是否开始                                      
	Activity_Data[BaoXiang_ID].Active_End,											-- 活动是否结束                                      
	Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime,			-- 宝箱可开启剩余时间（单位为秒）       
	Activity_Data[BaoXiang_ID].Tick_Count,											-- tick计时                                             
	Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime =		-- 重新刷新宝箱的倒计时（单位为秒）   
	act.GetActScriptData(
					DuoBaoQiBing_Active_ID,
					Need_Save_To_DB_Count,
					ScriptDataStartIdx,															-- 宝箱物品序列号（高位）                  
					ScriptDataStartIdx+1,														-- 宝箱物品序列号（低位）                  
					ScriptDataStartIdx+2,														-- 宝箱地物ID（高位）                         
					ScriptDataStartIdx+3,														-- 宝箱地物ID（低位）                         
					ScriptDataStartIdx+4,														-- 宝箱X坐标                                        
					ScriptDataStartIdx+5,														-- 宝箱Z坐标                                        
					ScriptDataStartIdx+6,														-- 拿到宝箱的玩家的角色ID                  
					ScriptDataStartIdx+7,														-- 分钟计数器                                       
					ScriptDataStartIdx+8,														-- 活动是否开始                                   
					ScriptDataStartIdx+9,														-- 活动是否结束                                   
					ScriptDataStartIdx+10	,													-- 宝箱可开启剩余时间（单位为秒）     
					ScriptDataStartIdx+11,													-- tick计时                                           
					ScriptDataStartIdx+12	)													-- 重新刷新宝箱的倒计时（单位为秒）
	act.SaveActScriptData(DuoBaoQiBing_Active_ID)

	Activity_Data[BaoXiang_ID].Save_To_DB = 0--]]
end

function ResetActiveDataWithBaoXiangID(BaoXiang_ID)
	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High = 0														-- 宝箱物品序列号（高位）
	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low = 0														-- 宝箱物品序列号（低位）
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High = 0														-- 宝箱地物ID（高位）
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low = 0														-- 宝箱地物ID（低位）
	Activity_Data[BaoXiang_ID].BaoXiang_X = 0																			-- 宝箱X坐标
	Activity_Data[BaoXiang_ID].BaoXiang_Z = 0																			-- 宝箱Z坐标
	Activity_Data[BaoXiang_ID].BaoXiang_Holder = 0																	-- 拿到宝箱的玩家的角色ID
	Activity_Data[BaoXiang_ID].Min_Counter = 0																		-- 分钟计数器
	Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime = BaoXiang_CDTime					-- 宝箱可开启剩余时间（单位为秒）
	Activity_Data[BaoXiang_ID].Tick_Count = 0																			-- tick计时
	Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval	-- 重新刷新宝箱的倒计时（单位为秒 ）

	Activity_Data[BaoXiang_ID].Save_To_DB = 1		--设置保存数据库
end

-- 重置活动相关变量
function ResetActiveData()
	ResetActiveDataWithBaoXiangID(BaoXiang_ID_M04)
	ResetActiveDataWithBaoXiangID(BaoXiang_ID_M07)
	ResetActiveDataWithBaoXiangID(BaoXiang_ID_M107)
end

-- 广播“XX玩家在XX场景XX坐标已经携带了宝物，请玩家抓紧时间去抢夺吧！”
function SendPickUpBaoXiangMsg(BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == nil or Activity_Data[BaoXiang_ID].BaoXiang_Holder == nil then
		return
	end
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == 0 or Activity_Data[BaoXiang_ID].BaoXiang_Holder == nil then
		return
	end
	if Activity_Data[BaoXiang_ID].BaoXiang_X == 0 and Activity_Data[BaoXiang_ID].BaoXiang_Z == 0 then
		-- 更新宝箱当前坐标
		UpdateBaoXiangPos(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)
		if Activity_Data[BaoXiang_ID].BaoXiang_X == 0 and Activity_Data[BaoXiang_ID].BaoXiang_Z == 0 then
			return
		end
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100202)
	msg.AddMsgEvent(MsgID, 2, Activity_Data[BaoXiang_ID].BaoXiang_Holder)
	msg.AddMsgEvent(MsgID, 6, Activity_Data[BaoXiang_ID].BaoXiang_MapID)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_X)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_Z)
	msg.DispatchWorldMsgEvent(MsgID)
end

--广播活动开始(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
function SendChaseUpdateMsg(BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == nil then
		return
	end
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == 0 then
		return
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100201)
	msg.AddMsgEvent(MsgID, 6, Activity_Data[BaoXiang_ID].BaoXiang_MapID)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_X)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_Z)
	msg.DispatchWorldMsgEvent(MsgID)
end

-- 宝箱持有者被玩家杀死
function SendChaseHolderBeKilledByRole(KillerID, BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == nil then
		return
	end
	
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == 0 then
		return
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100203)
	msg.AddMsgEvent(MsgID, 2, KillerID)
	msg.AddMsgEvent(MsgID, 2, Activity_Data[BaoXiang_ID].BaoXiang_Holder)
	msg.AddMsgEvent(MsgID, 6, Activity_Data[BaoXiang_ID].BaoXiang_MapID)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_X)
	msg.AddMsgEvent(MsgID, 9, Activity_Data[BaoXiang_ID].BaoXiang_Z)
	msg.DispatchWorldMsgEvent(MsgID)
end

-- 同步宝箱开启剩余时间
function SyncOpenChaseRemainTime(BaoXiang_ID)
	if BaoXiang_Holder ~= 0 then
		role.SyncOpenChaseRemainTime(Activity_Data[BaoXiang_ID].BaoXiang_Holder, Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime)
	end
end

-- 更新宝箱当前坐标()
function UpdateBaoXiangPos(RoleID, BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == nil then
		return
	end
	if Activity_Data[BaoXiang_ID].BaoXiang_MapID == 0 then
		return
	end

	if RoleID ~= 0 then
		local BaoXiang_Y = 0
		Activity_Data[BaoXiang_ID].BaoXiang_X, Activity_Data[BaoXiang_ID].BaoXiang_Y, Activity_Data[BaoXiang_ID].BaoXiang_Z = unit.GetPosition(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, RoleID)
		Activity_Data[BaoXiang_ID].Save_To_DB = 1		--设置保存数据库
	end
end

-- 去除角色身上的携宝模式状态
function UnSetHoldChestState(RoleID, BaoXiang_ID)
	role.UnSetRoleState(RoleID, ERS_HoldChest, 1)
	unit.CancelBuff(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, RoleID, HoldChest_BuffID)
end

-- 给角色设置夺宝奇兵活动携宝模式状态
function SetHoldChestState(RoleID, BaoXiang_ID)
	role.SetRoleState(RoleID, ERS_HoldChest, 1)
	unit.AddBuff(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, RoleID, HoldChest_BuffID, RoleID)
end

-- 设置拿到宝箱的玩家的角色ID
function SetBaoXiangHolderRoleID(RoleID, BaoXiang_ID)
	if RoleID == 0 then
		-- 去除角色身上的携宝模式状态
		UnSetHoldChestState(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)

		-- 如果宝箱被丢弃，重置宝箱开启剩余时间
		if BaoXiang_Hold_Time_Mod == 0 then
			Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime = BaoXiang_CDTime
		end

		-- 更新宝箱当前坐标
		UpdateBaoXiangPos(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)
	else
		-- 给角色设置夺宝奇兵活动携宝模式状态
		SetHoldChestState(RoleID, BaoXiang_ID)

		-- 重置刷新宝箱的倒计时（单位为秒 ）
		Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval

		-- 更新宝箱当前坐标
		UpdateBaoXiangPos(RoleID, BaoXiang_ID)
	end

	-- 设置拿到宝箱的玩家的角色ID
	Activity_Data[BaoXiang_ID].BaoXiang_Holder = RoleID

	-- 同步宝箱开启剩余时间
	SyncOpenChaseRemainTime(BaoXiang_ID)

	--设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

-- 删除活动宝箱
function RemoveBaoXiang(BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_Holder ~= 0 then
		role.RemoveFromRole(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID, 1, 310)
	else
		map.RemoveGroundItem(Activity_Data[BaoXiang_ID].BaoXiang_MapID, Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High, Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low)
	end
end

-- 每日活动开始
function ActiveStart(BaoXiang_ID, ActivityStartBroadcast)
	-- 如果前一天的活动没有结束，删掉宝箱
	--if Activity_Data[BaoXiang_ID].Active_End == 0 then
		-- 删除活动宝箱
		RemoveBaoXiang(BaoXiang_ID)
	--end

	-- 去除角色身上的携宝模式状态
	UnSetHoldChestState(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)

	-- 重置活动相关变量
	ResetActiveDataWithBaoXiangID(BaoXiang_ID)

	if ActivityStartBroadcast == true then
		-- 广播活动规则“携带宝物者不可进行使用"传送道具，下线、进入新手村、主城以及副本、下水"如果玩家进行以上操作或使用了规定中的道具，宝物将自动掉落在进行操作或使用道具前的地点，请玩家慎重！！！”
		local RuleMsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(RuleMsgID, 102, 100204)
		msg.DispatchWorldMsgEvent(RuleMsgID)
	end

	-- 将宝箱刷出来，放到随机地图里面的随机怪物的旁边
	-- 广播活动开始(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
	ActiveStartAndStartBroadcast(BaoXiang_ID)

	-- 活动状态标记
	Activity_Data[BaoXiang_ID].Active_Started = 1
	Activity_Data[BaoXiang_ID].Active_End = 0

	--设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

-- 夺宝奇兵活动结束
function ActiveEnd(BaoXiang_ID, SendGetItemBroadcast, SendEndBroadcast)
	Activity_Data[BaoXiang_ID].Active_End = 1

	if SendGetItemBroadcast == true then
		-- 广播“&lt;p1&gt;场景的夺宝奇兵活动已经结束，&lt;p2&gt;已经被&lt;p3&gt;持有时间超过&lt;p4&gt;分钟。”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100207)
		msg.AddMsgEvent(MsgID, 6, Activity_Data[BaoXiang_ID].BaoXiang_MapID)		--地图CRC
		msg.AddMsgEvent(MsgID, 4, BaoXiang_ID)															--宝箱TypeID
		msg.AddMsgEvent(MsgID, 2, Activity_Data[BaoXiang_ID].BaoXiang_Holder)		--roleid
		msg.AddMsgEvent(MsgID, 9, BaoXiang_CDTime/60)											--持有时间
		msg.DispatchWorldMsgEvent(MsgID)
	end
	if SendEndBroadcast == true then
		-- 发活动结束广播
		if Activity_Data[BaoXiang_ID_M04].Active_End == 1 and Activity_Data[BaoXiang_ID_M07].Active_End == 1 and Activity_Data[BaoXiang_ID_M107].Active_End == 1 then
			MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100205)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	end

	-- 去除角色身上的携宝模式状态
	UnSetHoldChestState(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)
end

function GetRandPosInMap(BaoXiang_ID)
	if BaoXiang_ID ~= BaoXiang_ID_M04 and BaoXiang_ID ~= BaoXiang_ID_M07 then
		return 0,0,0
	end
	RandNum = math.random(BaoXiang_Pos_MaxCnt)
	return BaoXiang_Pos[BaoXiang_ID][RandNum].x, BaoXiang_Pos[BaoXiang_ID][RandNum].y, BaoXiang_Pos[BaoXiang_ID][RandNum].z
end

-- 夺宝奇兵活动开始，刷宝箱出来并发广播
function ActiveStartAndStartBroadcast(BaoXiang_ID)
	MapID = 0
	X = 0
	Z = 0
	SerialID_High = 0
	SerialID_Low = 0
	GroupID_High = 0
	GroupID_Low = 0
	if BaoXiang_ID == BaoXiang_ID_M04 then
		BaoXiang_X, BaoXiang_Y, BaoXiang_Z= GetRandPosInMap(BaoXiang_ID)
		if BaoXiang_X == 0 and BaoXiang_Y == 0 and BaoXiang_Z == 0 then
			return
		end
		MapID, X, Z, SerialID_High, SerialID_Low, GroupID_High,	GroupID_Low = map.RefreshItemInMap(BaoXiang_ID, Activity_Data[BaoXiang_ID].BaoXiang_MapID, BaoXiang_X, BaoXiang_Y, BaoXiang_Z)
    elseif BaoXiang_ID == BaoXiang_ID_M07 then
		BaoXiang_X, BaoXiang_Y, BaoXiang_Z= GetRandPosInMap(BaoXiang_ID)
		if BaoXiang_X == 0 and BaoXiang_Y == 0 and BaoXiang_Z == 0 then
			return
		end
		MapID, X, Z, SerialID_High, SerialID_Low, GroupID_High,	GroupID_Low = map.RefreshItemInMap(BaoXiang_ID, Activity_Data[BaoXiang_ID].BaoXiang_MapID, BaoXiang_X, BaoXiang_Y, BaoXiang_Z)
    elseif BaoXiang_ID == BaoXiang_ID_M107 then
		MapID, X, Z, SerialID_High, SerialID_Low, GroupID_High,	GroupID_Low = map.PutItemNearRandCreature(BaoXiang_ID, Activity_Data[BaoXiang_ID].BaoXiang_MapID)
	else
		return
	end

	if MapID == nil or X == nil or Z == nil or SerialID_High==nil or SerialID_Low == nil or GroupID_High == nil or GroupID_Low == nil then
		return
	end

	--将宝箱刷出来，放到地图里面的随机怪物的旁边
	Activity_Data[BaoXiang_ID].BaoXiang_X							= X
	Activity_Data[BaoXiang_ID].BaoXiang_Z							= Z
	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High		= SerialID_High
	Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low			= SerialID_Low
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High		= GroupID_High
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low		= GroupID_Low

	--广播刷新宝箱(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
	SendChaseUpdateMsg(BaoXiang_ID)

	--设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

-- 判断夺宝奇兵活动是否开始了，并且还没有结束
function IsActiveEnd(BaoXiang_ID)
	return Activity_Data[BaoXiang_ID].Active_End
end

-- 更新宝箱开启剩余时间
function UpdateOpenRemainTime(BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime == nil then
		Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime	= BaoXiang_CDTime				-- 宝箱可开启剩余时间（单位为秒）
	end

	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_Holder ~= 0  then
		Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime = Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime-1

		-- 如果宝箱可开启剩余时间（单位为秒）小于0，夺宝奇兵活动结束
		if Activity_Data[BaoXiang_ID].BaoXiang_OpenRemainTime < 0 then
			ActiveEnd(BaoXiang_ID, true, true)
		end
	end
end

-- 更新重新刷新宝箱剩余时间
function BaoXiangUpdateRemainTime(BaoXiang_ID)
	if Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime == nil then
		Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime = BaoXiang_Update_Interval	    -- 重新刷新宝箱的倒计时（单位为秒 ）
	end
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_Holder == 0 then
		Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime = Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime-1

		-- 如果刷新宝箱剩余时间（单位为秒）小于0，重新刷新宝箱
		if Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime < 0 then
			map.RemoveGroundItem(Activity_Data[BaoXiang_ID].BaoXiang_MapID, Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High, Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low)

			-- 夺宝奇兵活动开始，刷宝箱出来并发广播
			ActiveStartAndStartBroadcast(BaoXiang_ID)

			-- 重新刷新宝箱的倒计时（单位为秒 ）
			Activity_Data[BaoXiang_ID].BaoXiang_UpdateRemainTime	= BaoXiang_Update_Interval

		end
	end
end

-- 检查是今天是否有夺宝奇兵活动
function IsTodayHaveActive()
	return WeekDay_OpenState[EWeekDay[os.date("%A")]]
end

-- 根据角色ID取宝箱ID
function GetBaoXiangIDByRoleID(RoleID)
	BaoXiang_ID = 0
	if Activity_Data[BaoXiang_ID_M04].BaoXiang_Holder==RoleID then
		BaoXiang_ID = BaoXiang_ID_M04
	elseif Activity_Data[BaoXiang_ID_M07].BaoXiang_Holder==RoleID then
		BaoXiang_ID = BaoXiang_ID_M07
	elseif Activity_Data[BaoXiang_ID_M107].BaoXiang_Holder==RoleID then
		BaoXiang_ID = BaoXiang_ID_M107
	end
	return BaoXiang_ID
end

function BaoXiang_OnActiveTimer(BaoXiang_ID, ActivityStartBroadcast)
	local Hour = tonumber(os.date("%H"))
	local Min  = tonumber(os.date("%M"))
	local Sec   = tonumber(os.date("%S"))

	-- tick计时（计算宝箱CD时间，单位为秒）
	Activity_Data[BaoXiang_ID].Tick_Count = Activity_Data[BaoXiang_ID].Tick_Count+1
	if Activity_Data[BaoXiang_ID].Tick_Count == 6 then

		-- 更新宝箱开启剩余时间
		UpdateOpenRemainTime(BaoXiang_ID)

		-- 更新重新刷新宝箱剩余时间
		BaoXiangUpdateRemainTime(BaoXiang_ID)

		Activity_Data[BaoXiang_ID].Tick_Count = 0
	end

	-- 如果到了结束时间了，还没有结束，强制结束
	if Activity_End_Hour == Hour and IsActiveEnd(BaoXiang_ID) == 0 then
		-- 删除活动宝箱
		RemoveBaoXiang(BaoXiang_ID)

		-- 夺宝奇兵活动结束
		ActiveEnd(BaoXiang_ID, false, true)
	end

	-- 活动开始前重置Active_Started
	if IsTodayHaveActive() == 1 then
		if Before_Activity_Broadcast == 0 and Hour == One_Hour_Before_Start_Hour and Min == 30 then
			Before_Activity_Broadcast = 1

			-- 广播活动开始前的公告
			local RuleMsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(RuleMsgID, 102, 100206)
			msg.DispatchWorldMsgEvent(RuleMsgID)
		end

		if Before_Activity_Broadcast == 1 and Hour == One_Hour_Before_Start_Hour and Min == 31 then
			Before_Activity_Broadcast = 0
		end

		if Hour == One_Hour_Before_Start_Hour and Min == 59 and Activity_Data[BaoXiang_ID].Active_Started == 1 then
			Activity_Data[BaoXiang_ID].Active_Started = 0
			
			--设置保存数据库
			Activity_Data[BaoXiang_ID].Save_To_DB = 1
		end

		if Hour == Activity_Start_Hour and Min == 0 and Activity_Data[BaoXiang_ID].Active_Started == 0 then
			ActiveStart(BaoXiang_ID, ActivityStartBroadcast)
		end
	end

	-- 保存活动数据到数据库
	SaveActivityDataToDB(BaoXiang_ID)
end

function BaoXiang_OnActiveTimeMin(BaoXiang_ID)
	
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	-- 如果宝箱还没有被玩家捡起
	if BaoXiang_Holder ~= 0 then
		-- 更新宝箱当前坐标
		UpdateBaoXiangPos(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)

		-- 同步宝箱开启剩余时间
		SyncOpenChaseRemainTime(BaoXiang_ID)
	end

	Activity_Data[BaoXiang_ID].Min_Counter = Activity_Data[BaoXiang_ID].Min_Counter+1
	if Activity_Data[BaoXiang_ID].Min_Counter > BaoXiang_Pos_Broadcast_Interval then

		-- 如果宝箱还没有被玩家捡起
		if Activity_Data[BaoXiang_ID].BaoXiang_Holder ~= 0 then
			-- 广播“XX玩家在XX场景XX坐标已经携带了宝物，请玩家抓紧时间去抢夺吧！”
			SendPickUpBaoXiangMsg(BaoXiang_ID)
		else
			-- 广播(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
			SendChaseUpdateMsg(BaoXiang_ID)
		end

		Activity_Data[BaoXiang_ID].Min_Counter = 0
	end
	
	--设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

function BaoXiang_OnPickUpBaoXiang(BaoXiang_ID, RoleID)
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	-- 设置拿到宝箱的玩家的角色ID
	SetBaoXiangHolderRoleID(RoleID, BaoXiang_ID)

	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High = 0
	Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low = 0

	-- 广播“XX玩家在XX场景XX坐标已经携带了宝物，请玩家抓紧时间去抢夺吧！”
	SendPickUpBaoXiangMsg(BaoXiang_ID)

	--设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

function BaoXiang_CanPickUpBaoXiang(BaoXiang_ID, RoleID)
	-- 检查等级
	local RoleLvl = role.GetRoleLevel(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, RoleID)
	if RoleLvl == nil then
		return 0		--E_Success
	end
	if Activity_Data[BaoXiang_ID].BaoXiang_ID == ItemID and  (RoleLvl < Active_Min_Level) then
		return E_Loot_Role_Level_Limit		-- 等级不满足要求
	end

	-- 检查角色游泳状态
	local CurMoveState = role.GetCurMoveState(RoleID)
	if CurMoveState == EMS_Swim	or CurMoveState == EMS_SwimStand then
		return E_Loot_Swim_Can_Not_Pickup_BaoXiang
	end

	-- 检查角色是不是身处安全区
	if role.IsRoleInStatus(RoleID, ERS_SafeArea) then
		return E_Loot_Role_SafeArea_Limit
	end

	-- 检查角色是否处于PK保护状态
	if role.IsRoleInStatus(RoleID, ERS_Safeguard) then
		return E_Loot_Role_Safeguard_Limit
	end

	-- 检查是否处于死亡状态
	if unit.IsInState(Activity_Data[BaoXiang_ID].BaoXiang_MapID, 0, RoleID, ES_Dead) then
		return 999 --不需要提示
	end

	return 0			--E_Success
end

function BaoXiang_OnChaseHolderDie(BaoXiang_ID, RoleID, KillerID)
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_Holder ~= RoleID then
		return
	end

	-- 更新宝箱当前坐标
	UpdateBaoXiangPos(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)

	if unit.IsPlayer(KillerID) then
		-- 发广播“<Msg id="100074" type="1" channel="5" value="玩家[&lt;p1&gt;]已经把携带宝物的玩家[&lt;p2&gt;]杀死，宝箱掉落在&lt;p3&gt;(&lt;p4&gt;,&lt;p5&gt;)坐标，玩家赶快去抢夺吧！"/>”
		SendChaseHolderBeKilledByRole(KillerID, BaoXiang_ID)
	else
		--广播刷新宝箱(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
		SendChaseUpdateMsg(BaoXiang_ID)
	end

	-- 将玩家身上的宝箱丢弃到地上
	role.PutDownItem(RoleID, BaoXiang_ID)

	-- 重置宝箱携带者ID
	SetBaoXiangHolderRoleID(0, BaoXiang_ID)

	-- 设置保存数据库
	Activity_Data[BaoXiang_ID].Save_To_DB = 1
end

function BaoXiang_CanDelBaoXiangOnGround(BaoXiang_ID, SerialID_High, SerialID_Low)
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return 0
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High == SerialID_High and Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low == SerialID_Low then
		return 1
	else
		return 0
	end
end

function BaoXiang_ForceLootChase(BaoXiang_ID, RoleID)
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_Holder == RoleID then
		-- 更新宝箱当前坐标
		UpdateBaoXiangPos(Activity_Data[BaoXiang_ID].BaoXiang_Holder, BaoXiang_ID)

		-- 将玩家身上的宝箱丢弃到地上
		Activity_Data[BaoXiang_ID].BaoXiang_GroupID_High, Activity_Data[BaoXiang_ID].BaoXiang_GroupID_Low = role.PutDownItem(RoleID, BaoXiang_ID)

		-- 通知玩家宝箱掉落
		role.ForceLootChase(Activity_Data[BaoXiang_ID].BaoXiang_Holder)

		-- 重置宝箱携带者ID
		SetBaoXiangHolderRoleID(0, BaoXiang_ID)

		--广播刷新宝箱(宝箱出现在XX场景、XX坐标点，大家赶快去抢夺宝物吧)
		SendChaseUpdateMsg(BaoXiang_ID)

		-- 设置保存数据库
		Activity_Data[BaoXiang_ID].Save_To_DB = 1
	end
end

function BaoXiang_CanPutDownBaoXiang(BaoXiang_ID, SerialID_High, SerialID_Low)
	if IsActiveEnd(BaoXiang_ID) == 1 then
		return 0
	end

	if Activity_Data[BaoXiang_ID].BaoXiang_SerialID_High == SerialID_High and Activity_Data[BaoXiang_ID].BaoXiang_SerialID_Low == SerialID_Low then
		return 1
	else
		return 0
	end
end

--------------------------------------------------------------------------------------
-- 脚本在C++程序里面注册的事件函数
--------------------------------------------------------------------------------------

-- 夺宝奇兵活动开始
function DuoBaoQiBing_OnActiveStart(ActiveID)
	if DuoBaoQiBing_trigger == 0 then
		return
	end

	WeekDay_OpenState[1], WeekDay_OpenState[2], WeekDay_OpenState[3], WeekDay_OpenState[4], WeekDay_OpenState[5], WeekDay_OpenState[6], WeekDay_OpenState[7] = act.GetDuoBaoQiBingOpenState()

	-- 重置活动相关变量
	ResetActiveData()

	-- 加载数据库中存储的活动数据
	--LoadActivityData(BaoXiang_ID_M04)
	--LoadActivityData(BaoXiang_ID_M07)
	--LoadActivityData(BaoXiang_ID_M107)

	Activity_Data[BaoXiang_ID_M04].Active_Started				= 0								-- 活动是否开始
	Activity_Data[BaoXiang_ID_M04].Active_End					= 1								-- 活动是否结束
	Activity_Data[BaoXiang_ID_M07].Active_Started				= 0								-- 活动是否开始
	Activity_Data[BaoXiang_ID_M07].Active_End					= 1								-- 活动是否结束
	Activity_Data[BaoXiang_ID_M107].Active_Started				= 0								-- 活动是否开始
	Activity_Data[BaoXiang_ID_M107].Active_End					= 1								-- 活动是否结束
end

-- 每tick更新
function DuoBaoQiBing_OnActiveTimer(ActiveID, Sec)
	if DuoBaoQiBing_trigger == 0 then
		return
	end
	BaoXiang_OnActiveTimer(BaoXiang_ID_M04, true)
	BaoXiang_OnActiveTimer(BaoXiang_ID_M07, false)
	BaoXiang_OnActiveTimer(BaoXiang_ID_M107, false)
end

-- 每分钟更新
function DuoBaoQiBing_OnActiveTimeMin(ActiveID)
	if DuoBaoQiBing_trigger == 0 then
		return
	end

	BaoXiang_OnActiveTimeMin(BaoXiang_ID_M04)
	BaoXiang_OnActiveTimeMin(BaoXiang_ID_M07)
	BaoXiang_OnActiveTimeMin(BaoXiang_ID_M107)
end

-- 活动结束
function DuoBaoQiBing_OnActiveEnd(ActiveID)

end

--玩家捡起宝箱，发广播
function DuoBaoQiBing_OnPickUpBaoXiang(ItemID, RoleID)
	if DuoBaoQiBing_trigger == 0 then
		return
	end

	if ItemID == BaoXiang_ID_M04 or ItemID == BaoXiang_ID_M07 or ItemID == BaoXiang_ID_M107 then
		BaoXiang_OnPickUpBaoXiang(ItemID, RoleID)
	end
end

--判断玩家是否能捡起宝箱
function DuoBaoQiBing_CanPickUpBaoXiang(ItemID, RoleID)
	if DuoBaoQiBing_trigger == 0 then
		return 0
	end

	if ItemID == BaoXiang_ID_M04 or ItemID == BaoXiang_ID_M07 or ItemID == BaoXiang_ID_M107 then
		return BaoXiang_CanPickUpBaoXiang(ItemID, RoleID)
	else
		return -1
	end
end

-- 持有宝箱的玩家被杀死
function DuoBaoQiBing_OnChaseHolderDie(RoleID, KillerID)
	if DuoBaoQiBing_trigger == 0 then
		return
	end

	BaoXiang_ID = GetBaoXiangIDByRoleID(RoleID)
	if BaoXiang_ID ~= 0 then
		BaoXiang_OnChaseHolderDie(BaoXiang_ID, RoleID, KillerID)
	end
end

-- 宝箱是否可以删除(0表示可以删除， 1表示不可以)
function DuoBaoQiBing_CanDelBaoXiangOnGround(TypeID, SerialID_High, SerialID_Low)
	if DuoBaoQiBing_trigger == 0 then
		return 0
	end

	if TypeID == BaoXiang_ID_M04 or TypeID == BaoXiang_ID_M07 or TypeID == BaoXiang_ID_M107 then
		return BaoXiang_CanDelBaoXiangOnGround(TypeID, SerialID_High, SerialID_Low)
	else
		return -1
	end
end

-- 宝箱强制掉落
function DuoBaoQiBing_ForceLootChase(RoleID)
	if DuoBaoQiBing_trigger == 0 then
		return
	end

	BaoXiang_ID = GetBaoXiangIDByRoleID(RoleID)
	if BaoXiang_ID ~= 0 then
		BaoXiang_ForceLootChase(BaoXiang_ID, RoleID)
	end
end

-- 服了，宝箱不能丢弃了(0表示可以， 1表示不可以)
function DuoBaoQiBing_CanPutDownBaoXiang(TypeID, SerialID_High, SerialID_Low)
	if DuoBaoQiBing_trigger == 0 then
		return 0
	end

	if TypeID ~= BaoXiang_ID_M04 and TypeID ~= BaoXiang_ID_M07 and TypeID ~= BaoXiang_ID_M107 then
		return 0
	end

	return BaoXiang_CanPutDownBaoXiang(TypeID, SerialID_High, SerialID_Low)
end

-- 宝箱是否能开启
function DuoBaoQiBing_CanOpenBaoXiang(MapID, InstanceID, TypeID, TargetID)
	if DuoBaoQiBing_trigger == 0 then
		return 0, 0
	end

	if TypeID ~= BaoXiang_ID_M04 and TypeID ~= BaoXiang_ID_M07 and TypeID ~= BaoXiang_ID_M107 then
		return 0, 0
	end

	if IsActiveEnd(TypeID) == 1 then
		local FreeSize = role.GetBagFreeSize(TargetID)
		if(FreeSize < Open_Chase_Need_Free_Space[TypeID].Free_Space) then
			--提示玩家背包空间不足
			return 40, false
		end
		return 0, 0
	end

	-- E_UseItem_TreasureInTime		=	61,		// 夺宝奇兵，尚未到达活动结束时间
	return 61, 1
end

--角色初始化时候，检查背包中物品需要删除（0表示需要删除， 1表示不需要）
function DuoBaoQiBing_CanDeleteBaoXiang(TypeID, SerialID_High, SerialID_Low)
	if TypeID ~= BaoXiang_ID_M04 and TypeID ~= BaoXiang_ID_M07 and TypeID ~= BaoXiang_ID_M107 then
		return 1
	end

	if Activity_Data[TypeID].BaoXiang_SerialID_High == SerialID_High and Activity_Data[TypeID].BaoXiang_SerialID_Low == SerialID_Low and IsActiveEnd(TypeID) == 1 then
		return 1
	else
		return 0
	end
end

--活动控制台开关
function DuoBaoQiBing_OnActOpenClose(ActiveID, IsOpen)
	if DuoBaoQiBing_Active_ID ~= ActiveID then
		return
	end
	
	if DuoBaoQiBing_trigger == 0 and IsActiveEnd(BaoXiang_ID) ~= 1 then
		-- 删除活动宝箱
		RemoveBaoXiang(BaoXiang_ID_M04)    
		RemoveBaoXiang(BaoXiang_ID_M07)    
		RemoveBaoXiang(BaoXiang_ID_M107)  

		-- 夺宝奇兵活动结束
		ActiveEnd(BaoXiang_ID_M04, false, true)
		ActiveEnd(BaoXiang_ID_M07, false, true)
		ActiveEnd(BaoXiang_ID_M107, false, true)
	end

	DuoBaoQiBing_trigger = IsOpen
end

-- 向活动注册初始化和适时更新操作
aux.RegisterActEvent(DuoBaoQiBing_Active_ID, ESAE_OnTimer, "DuoBaoQiBing_OnActiveTimer")
aux.RegisterActEvent(DuoBaoQiBing_Active_ID, ESAE_OnStart, "DuoBaoQiBing_OnActiveStart")
aux.RegisterActEvent(DuoBaoQiBing_Active_ID, ESAE_OnEnd, "DuoBaoQiBing_OnActiveEnd")
aux.RegisterActEvent(DuoBaoQiBing_Active_ID, ESAE_OnTimerMin, "DuoBaoQiBing_OnActiveTimeMin")
aux.RegisterActEvent(DuoBaoQiBing_Active_ID, ESAE_OnActOpenClose, "DuoBaoQiBing_OnActOpenClose")

aux.RegisterRoleEvent(ESRE_OnChaseHolderDie,	"DuoBaoQiBing_OnChaseHolderDie")
aux.RegisterRoleEvent(ESRE_ForceLootChase,		"DuoBaoQiBing_ForceLootChase")

aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_CanPickUpItem,					"DuoBaoQiBing_CanPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_PickUpItem,							"DuoBaoQiBing_OnPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_CanDelGroundItem,				"DuoBaoQiBing_CanDelBaoXiangOnGround")
--aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_PutDownItem,					"DuoBaoQiBing_PutDownItem")
aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_CanPutDown,						"DuoBaoQiBing_CanPutDownBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_CanUse,								"DuoBaoQiBing_CanOpenBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M04, ESIE_CanDelItemOnRoleInit,		"DuoBaoQiBing_CanDeleteBaoXiang")

aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_CanPickUpItem,					"DuoBaoQiBing_CanPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_PickUpItem,							"DuoBaoQiBing_OnPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_CanDelGroundItem,				"DuoBaoQiBing_CanDelBaoXiangOnGround")
--aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_PutDownItem,					"DuoBaoQiBing_PutDownItem")
aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_CanPutDown,						"DuoBaoQiBing_CanPutDownBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_CanUse,								"DuoBaoQiBing_CanOpenBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M07, ESIE_CanDelItemOnRoleInit,		"DuoBaoQiBing_CanDeleteBaoXiang")

aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_CanPickUpItem,					"DuoBaoQiBing_CanPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_PickUpItem,							"DuoBaoQiBing_OnPickUpBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_CanDelGroundItem,				"DuoBaoQiBing_CanDelBaoXiangOnGround")
--aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_PutDownItem,					"DuoBaoQiBing_PutDownItem")
aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_CanPutDown,						"DuoBaoQiBing_CanPutDownBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_CanUse,								"DuoBaoQiBing_CanOpenBaoXiang")
aux.RegisterItemEvent(BaoXiang_ID_M107, ESIE_CanDelItemOnRoleInit,		"DuoBaoQiBing_CanDeleteBaoXiang")

