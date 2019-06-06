
--幻境之辉 用于孵化羽灵蛋

--[[
6010803	微弱幻境之辉 资质 10~40
6010804	闪亮幻境之辉 资质 50~70
6010805	耀眼幻境之辉 资质 80~90
6010806	辉煌幻境之辉 资质 100
]]
function Huanjingzhihui_canUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)	--判断背包空闲空间是否足够
	if (FreeSize < 1) then
		bRet = 40	--提示玩家背包空间不足
		return bRet, bIgnore
	else
		local Num = 0
		local ZiZhi = 0
		for k = 1 ,16 do
			Num =  role.GetRoleItemNum(RoleID,6010000+k) + Num
			Num =  role.GetRoleItemNum(RoleID,6010100+k) + Num
			Num =  role.GetRoleItemNum(RoleID,6010200+k) + Num
			Num =  role.GetRoleItemNum(RoleID,6010300+k) + Num
		end
		if Num == 0 then
			bRet = 32	--物品不可使用
			return bRet, bIgnore
		else
			if  TypeID == 6010803 then
				for i = 1, 16 do
					if role.GetRoleItemEnmity(RoleID,6010000+i) >= 10 and  role.GetRoleItemEnmity(RoleID,6010000+i) <= 40 or role.GetRoleItemEnmity(RoleID,6010100+i) >= 10 and  role.GetRoleItemEnmity(RoleID,6010100+i) <= 40 or role.GetRoleItemEnmity(RoleID,6010200+i) >= 10 and  role.GetRoleItemEnmity(RoleID,6010200+i) <= 40 or role.GetRoleItemEnmity(RoleID,6010300+i) >= 10 and  role.GetRoleItemEnmity(RoleID,6010300+i) <= 40 then	 -- 获取羽灵蛋资质
						ZiZhi = ZiZhi + 1
					end
				end
				if ZiZhi > 0 then
					return bRet, bIgnore
				else
					bRet = 32	--物品不可使用
					return bRet, bIgnore
				end
			elseif TypeID == 6010804 then
				for i = 1, 16 do
					if role.GetRoleItemEnmity(RoleID,6010000+i) >= 50 and  role.GetRoleItemEnmity(RoleID,6010000+i) <= 70 or role.GetRoleItemEnmity(RoleID,6010100+i) >= 50 and  role.GetRoleItemEnmity(RoleID,6010100+i) <= 70 or role.GetRoleItemEnmity(RoleID,6010200+i) >= 50 and  role.GetRoleItemEnmity(RoleID,6010200+i) <= 70 or role.GetRoleItemEnmity(RoleID,6010300+i) >= 50 and  role.GetRoleItemEnmity(RoleID,6010300+i) <= 70 then	 -- 获取羽灵蛋资质
						ZiZhi = ZiZhi + 1
					end
				end
				if ZiZhi > 0 then
					return bRet, bIgnore
				else
					bRet = 32	--物品不可使用
					return bRet, bIgnore
				end
			elseif TypeID == 6010805 then
				for i = 1, 16 do
					if role.GetRoleItemEnmity(RoleID,6010000+i) >= 80 and  role.GetRoleItemEnmity(RoleID,6010000+i) <= 90 or role.GetRoleItemEnmity(RoleID,6010100+i) >= 80 and  role.GetRoleItemEnmity(RoleID,6010100+i) <= 90 or role.GetRoleItemEnmity(RoleID,6010200+i) >= 80 and  role.GetRoleItemEnmity(RoleID,6010200+i) <= 90 or role.GetRoleItemEnmity(RoleID,6010300+i) >= 80 and  role.GetRoleItemEnmity(RoleID,6010300+i) <= 90 then	 -- 获取羽灵蛋资质
						ZiZhi = ZiZhi + 1
					end
				end
				if ZiZhi > 0 then
					return bRet, bIgnore
				else
					bRet = 32	--物品不可使用
					return bRet, bIgnore
				end
			elseif TypeID == 6010806 then
				for i = 1, 16 do
					if role.GetRoleItemEnmity(RoleID,6010000+i) == 100  or role.GetRoleItemEnmity(RoleID,6010100+i) == 100 or role.GetRoleItemEnmity(RoleID,6010200+i) == 100 or role.GetRoleItemEnmity(RoleID,6010300+i) == 100 then	 -- 获取羽灵蛋资质
						ZiZhi = ZiZhi + 1
					end
				end
				if ZiZhi > 0 then
					return bRet, bIgnore
				else
					bRet = 32	--物品不可使用
					return bRet, bIgnore
				end
			end
		end
	end
end
aux.RegisterItemEvent(6010803, 0, "Huanjingzhihui_canUse")
aux.RegisterItemEvent(6010804, 0, "Huanjingzhihui_canUse")
aux.RegisterItemEvent(6010805, 0, "Huanjingzhihui_canUse")
aux.RegisterItemEvent(6010806, 0, "Huanjingzhihui_canUse")

--羽灵蛋孵化

function Huanjingzhihui_OnUse(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local ZiZhi_qian = 0
 	local ZiZhi_kun = 0
	local ZiZhi_kan = 0
	local ZiZhi_li = 0
	if TypeID == 6010803 then
		local Index_1 = math.random(100)
		for i = 1 ,4 do
			ZiZhi_qian = role.GetRoleItemEnmity(RoleID,6010000 +i)
			ZiZhi_kun = role.GetRoleItemEnmity(RoleID,6010100 +i)
			ZiZhi_kan = role.GetRoleItemEnmity(RoleID,6010200 +i)
			ZiZhi_li = role.GetRoleItemEnmity(RoleID,6010300 +i)
			if ZiZhi_qian > 0 then
				if  ZiZhi_qian >= 10 and  ZiZhi_qian <= 40 then
					if Index_1 > 0 and Index_1 <= 75 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+i, 1, 2, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kun > 0 then
				if  ZiZhi_kun >= 10 and  ZiZhi_kun <= 40 then
					if Index_1 > 0 and Index_1 <= 75 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+i, 1, 2, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kan > 0 then
				if  ZiZhi_kan >= 10 and  ZiZhi_kan <= 40 then
					if Index_1 > 0 and Index_1 <= 75 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010200+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710200+i, 1, 2, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_li > 0 then
				if  ZiZhi_li >= 10 and  ZiZhi_li <= 40 then
					if Index_1 > 0 and Index_1 <= 75 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010300+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710300+i, 1, 2, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			end
		end
	elseif  TypeID == 6010804 then
		local Index_2 = math.random(100)
		for k = 5 ,7 do
			ZiZhi_qian = role.GetRoleItemEnmity(RoleID,6010000 +k)
			ZiZhi_kun = role.GetRoleItemEnmity(RoleID,6010100 +k)
			ZiZhi_kan = role.GetRoleItemEnmity(RoleID,6010200 +k)
			ZiZhi_li = role.GetRoleItemEnmity(RoleID,6010300 +k)
			if ZiZhi_qian > 0 then
				if  ZiZhi_qian >= 50 and  ZiZhi_qian <= 70 then
					if Index_2 > 0 and Index_2 <= 50 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+k,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+k, 1, 3, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kun > 0 then
				if  ZiZhi_kun >= 50 and  ZiZhi_kun <= 70 then
					if  Index_2 > 0 and Index_2 <= 50 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+k,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+k, 1, 3, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kan > 0 then
				if  ZiZhi_kan >= 50 and  ZiZhi_kan <= 70 then
					if  Index_2 > 0 and Index_2 <= 50 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010200+k,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710200+k, 1, 3, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_li > 0 then
				if  ZiZhi_li >= 50 and  ZiZhi_li <= 70 then
					if  Index_2 > 0 and Index_2 <= 50 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010300+k,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710300+k, 1, 3, 8, 420)
						local MsgID = msg.BeginMsgEvent() -- 孵化成功
						msg.AddMsgEvent(MsgID, 26, 4306)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			end
		end
	elseif  TypeID == 6010805 then
		local Index_3 = math.random(100)
		local  x = {}
				x[1] = 8
				x[2] = 9
				x[3] = 11
				x[4] = 12
				x[5] = 14
				x[6] = 15
		for i = 8,9 do
			ZiZhi_kan = role.GetRoleItemEnmity(RoleID,6010200 +i)
			ZiZhi_li = role.GetRoleItemEnmity(RoleID,6010300 +i)
			if ZiZhi_kan > 0 then
				if ZiZhi_kan >= 80 and ZiZhi_kan <= 90 then
					if  Index_3 > 0 and Index_3 <= 30 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010200+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710200+i, 1, 4, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305)
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 6010200+i)
						msg.AddMsgEvent(MsgID, 4, 8710200+i)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_li > 0 then
				if  ZiZhi_li >= 80 and  ZiZhi_li <= 90 then
					if  Index_3 > 0 and Index_3 <= 30 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010300+i,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710300+i, 1, 4, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305) -- 孵化成功，世界公告
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 6010300+i)
						msg.AddMsgEvent(MsgID, 4, 8710300+i)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			end
		end
		for l,v in ipairs(x)  do
			ZiZhi_qian = role.GetRoleItemEnmity(RoleID,6010000 +v)
			ZiZhi_kun = role.GetRoleItemEnmity(RoleID,6010100 +v)
			if ZiZhi_qian > 0 then
				if  ZiZhi_qian >= 80 and  ZiZhi_qian <= 90 then
					if  Index_3 > 0 and Index_3 <= 30 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+v,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+v, 1, 4, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305)
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 6010000+v)
						msg.AddMsgEvent(MsgID, 4, 8710000+v)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kun > 0 then
				if  ZiZhi_kun >= 80 and  ZiZhi_kun <= 90 then
					if  Index_3 > 0 and Index_3 <= 30 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+v,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+v, 1, 4, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305) -- 孵化成功，世界公告
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 6010000+v)
						msg.AddMsgEvent(MsgID, 4, 8710100+v)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			end
		end
	elseif  TypeID == 6010806 then
		local Index_4 = math.random(100)
		local y = {}
				y[1] = 10
				y[2] = 13
				y[3] = 16
		ZiZhi_kan = role.GetRoleItemEnmity(RoleID,6010210)
		ZiZhi_li = role.GetRoleItemEnmity(RoleID,6010310)
		if ZiZhi_kan > 0 then
			if ZiZhi_kan == 100 then
				if  Index_4 > 0 and Index_4 <= 10 then
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010210,1,420)
					role.AddRoleItem(MapID, InstanceID, RoleID, 8710210, 1, 5, 8, 420)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 4305)
					msg.AddMsgEvent(MsgID, 2, RoleID)-- 孵化成功，世界公告
					msg.AddMsgEvent(MsgID, 4, 6010210)
					msg.AddMsgEvent(MsgID, 4, 8710210)
					msg.DispatchWorldMsgEvent(MsgID)
				else
					local MsgID = msg.BeginMsgEvent() -- 孵化失败
					msg.AddMsgEvent(MsgID, 26, 4307)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		elseif ZiZhi_li > 0 then
			if ZiZhi_li == 100 then
				if  Index_4 > 0 and Index_4 <= 10 then
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010310,1,420)
					role.AddRoleItem(MapID, InstanceID, RoleID, 8710310, 1, 5, 8, 420)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 4305)
					msg.AddMsgEvent(MsgID, 2, RoleID)-- 孵化成功，世界公告
					msg.AddMsgEvent(MsgID, 4, 6010310)
					msg.AddMsgEvent(MsgID, 4, 8710310)
					msg.DispatchWorldMsgEvent(MsgID)
				else
					local MsgID = msg.BeginMsgEvent() -- 孵化失败
					msg.AddMsgEvent(MsgID, 26, 4307)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end
		for m ,v in ipairs(y) do
			ZiZhi_qian = role.GetRoleItemEnmity(RoleID,6010000 +v)
			ZiZhi_kun = role.GetRoleItemEnmity(RoleID,6010100 +v)
			if ZiZhi_qian > 0 then
				if  ZiZhi_qian == 100 then
					if  Index_4 > 0 and Index_4 <= 10 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+v,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+v, 1, 5, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305)
						msg.AddMsgEvent(MsgID, 2, RoleID)-- 孵化成功，世界公告
						msg.AddMsgEvent(MsgID, 4, 6010000+v)
						msg.AddMsgEvent(MsgID, 4, 8710000+v)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			elseif ZiZhi_kun > 0 then
				if  ZiZhi_kun == 100 then
					if  Index_4 > 0 and Index_4 <= 10 then
						role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+v,1,420)
						role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+v, 1, 5, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 4305)
						msg.AddMsgEvent(MsgID, 2, RoleID)
						msg.AddMsgEvent(MsgID, 4, 6010000+v)
						msg.AddMsgEvent(MsgID, 4, 8710000+v)
						msg.DispatchWorldMsgEvent(MsgID)
						break
					else
						local MsgID = msg.BeginMsgEvent() -- 孵化失败
						msg.AddMsgEvent(MsgID, 26, 4307)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						break
					end
				end
			end
		end
	end
end

aux.RegisterItemEvent(6010803, 1, "Huanjingzhihui_OnUse")
aux.RegisterItemEvent(6010804, 1, "Huanjingzhihui_OnUse")
aux.RegisterItemEvent(6010805, 1, "Huanjingzhihui_OnUse")
aux.RegisterItemEvent(6010806, 1, "Huanjingzhihui_OnUse")





































