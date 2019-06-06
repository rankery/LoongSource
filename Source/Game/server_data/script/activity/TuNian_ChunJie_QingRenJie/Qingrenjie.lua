
--RoleDataType["Qingrenjie_Yutu"]	= 163  --记录每个玩家喂养的兔子ID
--RoleDataType["Qingrenjie_YutuJiangli"]	= 164  --记录每个玩家的领取奖励次数

--活动开启
function Qingrenjie_OnStart(actID)
	if Qingrenjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100505)	--情人节来临之际，《龙》祝天下有情人双宿双飞，终成眷属！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		CreatureID_Qingren1 = map.MapCreateCreature(3017298383, -1, 4900720, 765, 10000, 2086)  -- 凤翔刷月宫玉女
		CreatureID_Qingren2 = map.MapCreateCreature(3017298383, -1, 4900722, 765, 10000, 2078)	-- 凤翔刷远玄
		CreatureID_Qingren3 = map.MapCreateCreature(3017298895, -1, 4900721, 1064, 6651, 1113)	-- 婵娟池刷青烛
	end
end
--活动结束
function Qingrenjie_OnEnd(actID)
	if Qingrenjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100506)	--情人节活动已经结束，感谢各位的积极参与，并祝天下所有情侣幸福美满！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		map.MapDeleteCreature(3017298383, -1, CreatureID_Qingren1)
		map.MapDeleteCreature(3017298383, -1, CreatureID_Qingren2)
		map.MapDeleteCreature(3017298895, -1, CreatureID_Qingren3)
	end
end
--玉兔笼是否可使用
function I2616511_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3017298895 then
		bRet = 32
	end
	return bRet, bIgnore
end
--玉兔笼使用
function I2616511_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	if MapID == 3017298895 then
		local shanpao = role.GetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_YutuJiangli"])
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		CreatureID_Yutu = map.MapCreateCreature(3017298895, -1, 4900723, x+1,y,z+1)
		role.SetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_Yutu"],CreatureID_Yutu)
		cre.SetCreatureScriptData(MapID, InstanceID, CreatureID_Yutu, 1, 1, RoleID)--记录喂养者的id
		role.SetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_YutuJiangli"],shanpao+1)--记录喂养者喂养兔子总数
	end
end
--精品礼盒使用
function I2616514_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 2616516, 1, -1, 8, 420)
	role.AddRoleSilver(MapID, InstanceID, RoleID, level*1000, 102)
end
--精品礼盒使用
function I2616515_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 2616516, 1, -1, 8, 420)
	role.AddRoleSilver(MapID, InstanceID, RoleID, level*1000, 102)
end
--判断信函是否可用
function I2616517_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet,bIgnore = 0,false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	if MapID ~= 3017298895 or x > 1060 or x < 1040 or z > 1060 or z < 1040 then
		bRet = 43
	end
	return bRet,bIgnore
end
--使用信函
function I2616517_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	map.MapCreateCreature(3017298895, -1, 1009235, 1050, 6078, 1050)  -- 刷1009235丧情夜魔
end
--判断精品礼盒是否可赠送给对方
function I2616514_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet,bIgnore = 0,false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if role.GetRoleSex(TargetID) ~= 1 then
		bRet = 34
	elseif FreeSize < 1 then
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end
	return bRet, bIgnore
end
--判断精品礼盒是否可赠送给对方
function I2616515_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet,bIgnore = 0,false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if role.GetRoleSex(TargetID) ~= 0 then
		bRet = 34
	elseif FreeSize < 1 then
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	end
	return bRet, bIgnore
end
--喂养兔子过程
function Yutu_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local seedowner = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1)--所属者ID
	local growNum = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2)--喂养者喂养其次数
	if RoleID == seedowner then
		if TalkIndex == -1 then
			if growNum < 5 then
				if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3) == 0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 12604)	-- "小玉兔刚刚出生，需要你的细心照料我才能茁壮成长！"
					msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 12602)	-- "喂它胡萝卜"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
					msg.AddMsgEvent(MsgID, 1, 12603)	-- "离开"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local cTime = os.time()
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --距离上一次对话时间
					if s < 120 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 12601) -- 不要喂我吃的太快哦，吃多了会撑出病来的！每次喂养我的间隔一定要在2分钟以上！
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
						msg.AddMsgEvent(MsgID, 1, 12603)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 12605) -- 主人主人，我又饿了！
						msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
						msg.AddMsgEvent(MsgID, 1, 12602)	-- "喂它胡萝卜"
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
						msg.AddMsgEvent(MsgID, 1, 12603)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				end
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12606)  -- 谢谢你，善良的主人，我要回到月亮上去了，这是给您留下的礼物，请收下！
				msg.AddMsgEvent(MsgID, 21, 6)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 12607)	-- "玉兔的礼物"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif TalkIndex == 4 then
			if role.GetRoleItemNum(RoleID, 2616512) > 0 then
				if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3) == 0 then
					local lTime = os.time()
					cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3,lTime)
					cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2,growNum+1)
					unit.AddBuff(MapID, InstanceID, TargetID, 5002801, TargetID)
					role.RemoveFromRole(MapID,InstanceID,RoleID,2616512,1,420)
					local k = math.random(100)
					if k <= 15 then
						role.AddRoleItem(MapID,InstanceID,RoleID,2616513,1,-1,8,420)
					end
				else
					local cTime = os.time() --当前时间
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --距离上一次对话时间
					if s >= 120 then  --间隔大于2分钟
						unit.AddBuff(MapID, InstanceID, TargetID, 5002801, TargetID)
						cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3,cTime)
						cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2,growNum+1)
						role.RemoveFromRole(MapID,InstanceID,RoleID,2616512,1,420)
						local k = math.random(100)
						if k <= 15 then
							role.AddRoleItem(MapID,InstanceID,RoleID,2616513,1,-1,8,420)
						end
					end
				end
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12610)  --主人，您没有胡萝卜了，我好饿啊！
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif TalkIndex == 6 then					--5次喂养过后操作
			local Qingrenjie_YutuJiangli = role.GetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_YutuJiangli"])
			if Qingrenjie_YutuJiangli < 1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12609)  --走开！你不是我的主人！
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif role.GetBagFreeSize(RoleID) < 1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12608) --您的背包空间不足!
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 12603)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif role.GetRoleSex(RoleID) == 0 then
				local Qingrenjie_Yutu = role.GetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_Yutu"])
				role.AddRoleItem(MapID,InstanceID,RoleID,2616514,1,-1,8,420)
				map.MapDeleteCreature(3017298895, -1, Qingrenjie_Yutu)
				role.SetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_YutuJiangli"],Qingrenjie_YutuJiangli-1)
			elseif role.GetRoleSex(RoleID) == 1 then
				role.AddRoleItem(MapID,InstanceID,RoleID,2616515,1,-1,8,420)
				local Qingrenjie_Yutu = role.GetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_Yutu"])
				map.MapDeleteCreature(3017298895, -1, Qingrenjie_Yutu)
				role.SetRoleScriptData(RoleID,1,RoleDataType["Qingrenjie_YutuJiangli"],Qingrenjie_YutuJiangli-1)
			end
		end
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12609)  --走开！你不是我的主人！
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	end
end
aux.RegisterCreatureEvent(4900723, 7, "Yutu_OnTalk")
aux.RegisterActEvent(160, 2, "Qingrenjie_OnStart")
aux.RegisterActEvent(160, 3, "Qingrenjie_OnEnd")
aux.RegisterItemEvent(2616511, 1, "I2616511_OnUse")
aux.RegisterItemEvent(2616511, 0, "I2616511_CanUse")
aux.RegisterItemEvent(2616514, 0, "I2616514_CanUse")
aux.RegisterItemEvent(2616515, 0, "I2616515_CanUse")
aux.RegisterItemEvent(2616514, 1, "I2616514_OnUse")
aux.RegisterItemEvent(2616515, 1, "I2616515_OnUse")
aux.RegisterItemEvent(2616517, 1, "I2616517_OnUse")
aux.RegisterItemEvent(2616517, 0, "I2616517_CanUse")
