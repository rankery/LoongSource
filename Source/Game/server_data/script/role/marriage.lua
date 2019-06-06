-- 夫妻系统需要的一些全局定义

--结婚好友度限制
marriage_FriendShipVal  = 720  

--申请者结婚金钱 银
marriage_GetMarriageDecSilver = 660000
--申请者离婚金钱 银
marriage_BreakMarriageDecSilver = 1000000
--申请婚礼金钱 银
marriage_WeddingDecSilver = 1000000

--1361107	月老之红线
--1361108	红娘之红线
--1361109	未开封的婚戒
--1361110	未开封的礼服
--1361111	大婚礼包      男的
--1361112	大婚礼包      女的
--8990101	[婚]君子
--8990102	[婚]君子
--8990103	[婚]君子
--8990111	[婚]淑女
--8990112	[婚]淑女
--8990113	[婚]淑女

--结婚
function Marriage_GetMarriageEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--删除婚前未开封的物品
	role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 1361109 , 1, 101)
	role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 1361110 , 1, 101)
	
	role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 1361109 , 1, 101)
	role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 1361110 , 1, 101)
	
	--添加婚后物品 0:girl，1:boy(便于以后扩展)
	--男方 	戒指：君子	物品包：男性礼服套装
	--女方	戒指：淑女	物品包：女性婚纱套装
	local MaleRoleID = 0 --男方的roleid
	local FeMaleRoleID = 0 --女方的roleid

	if role.GetRoleSex(ApplicantRoleID) == 0 then
		--女
		role.AddRoleItem(MapID, InstanceID, ApplicantRoleID, 1361112	, 1, -1, 3, 104)
		role.AddRoleItem(MapID, InstanceID, ApplicantRoleID, 8990111	, 1, -1, 3, 104)

		--男
		role.AddRoleItem(MapID, InstanceID, TargetRoleID, 1361111	, 1, -1, 3, 104)
		role.AddRoleItem(MapID, InstanceID, TargetRoleID, 8990101	, 1, -1, 3, 104)

		MaleRoleID = TargetRoleID
		FeMaleRoleID = ApplicantRoleID
	else
		--男
		role.AddRoleItem(MapID, InstanceID, ApplicantRoleID, 1361111	, 1, -1, 3, 104)
		role.AddRoleItem(MapID, InstanceID, ApplicantRoleID, 8990101	, 1, -1, 3, 104)
		--女
		role.AddRoleItem(MapID, InstanceID, TargetRoleID, 1361112	, 1, -1, 3, 104)
		role.AddRoleItem(MapID, InstanceID, TargetRoleID, 8990111	, 1, -1, 3, 104)

		MaleRoleID = ApplicantRoleID
		FeMaleRoleID = TargetRoleID
	end

	--减申请者的钱钱
	role.DecRoleSilver(MapID, InstanceID, ApplicantRoleID, marriage_GetMarriageDecSilver, 101)
	
	--全服广播
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 500)	-- 全服广播###（男方）与###（女方）正式结为夫妻，祝他们白头携老，永结同心！
	msg.AddMsgEvent(MsgID, 2, MaleRoleID)	-- 男方roleid
	msg.AddMsgEvent(MsgID, 2, FeMaleRoleID)	-- 女方roleid
	msg.DispatchWorldMsgEvent(MsgID)

end

--结婚检查
function Marriage_GetMarriageCheckEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--性别是否相同
	if role.GetRoleSex(ApplicantRoleID) == role.GetRoleSex(TargetRoleID) then
		return 2 --性别相同
	end
	
	--检查好友度
	if role.GetFriendShipValue(ApplicantRoleID,TargetRoleID) < marriage_FriendShipVal then
		return 3 --好友度不够
	end
	
	--检查申请者是否已婚
	if role.IsRoleMarried(ApplicantRoleID) == true then 
		return 4 --申请者已婚
	end

	if role.IsRoleMarried(TargetRoleID) == true then 
		return 5 --被申请者已婚
	end

	--申请者金钱是否足够
	if role.GetRoleSilver(MapID,InstanceID,ApplicantRoleID) < marriage_GetMarriageDecSilver then
		return 7 --申请者必须拥有66金
	end

	--是否有婚前未开封的物品
	local num1 = role.GetRoleItemNum(ApplicantRoleID,1361109)
	local num2 = role.GetRoleItemNum(ApplicantRoleID,1361110)

	local num3 = role.GetRoleItemNum(TargetRoleID,1361109)
	local num4 = role.GetRoleItemNum(TargetRoleID,1361110)

	if num1<1 or num2<1 or num3<1 or num4 <1 then
		return  10 --缺少必要物品
	end

	local num5 = role.GetBagFreeSize(ApplicantRoleID)
	local num6 = role.GetBagFreeSize(TargetRoleID)

	if num5<2 or num6<2 then
		return 12 --背包空间不够
	end

	return 0 --成功

end

--删除结婚戒指
function Marriage_DelRing(MapID, InstanceID, ApplicantRoleID, TargetRoleID)
	--删除婚后物品 0:girl，1:boy(便于以后扩展)
	--男方 	戒指：君子
	--女方	戒指：淑女
	--戒指有可能已经升级了 所以要把所有款式的戒指都有删一遍
	if role.GetRoleSex(ApplicantRoleID) == 0 then
		--女背包
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990111	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990112	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990113	, 1, -1, 3, 104)
		--女仓库
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990111	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990112	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990113	, 1, -1, 3, 104)
		--女装备栏
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990111	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990112	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990113	, 1, -1, 3, 104)
		--男背包
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990101	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990102	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990103	, 1, -1, 3, 104)
		--男仓库
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990101	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990102	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990103	, 1, -1, 3, 104)
		--男装备栏
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990101	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990102	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990103	, 1, -1, 3, 104)
	else
		--男背包
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990101	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990102	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 8990103	, 1, -1, 3, 104)
		--男仓库
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990101	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990102	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, ApplicantRoleID, 8990103	, 1, -1, 3, 104)
		--男装备栏
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990101	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990102	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, ApplicantRoleID, 8990103	, 1, -1, 3, 104)
		--女背包
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990111	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990112	, 1, -1, 3, 104)
		role.RemoveFromRole(MapID, InstanceID, TargetRoleID, 	8990113	, 1, -1, 3, 104)
		--女仓库
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990111	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990112	, 1, -1, 3, 104)
		role.RemoveFromWare(MapID, InstanceID, TargetRoleID, 	8990113	, 1, -1, 3, 104)
		--女装备栏
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990111	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990112	, 1, -1, 3, 104)
		role.RemoveFromEquipBar(MapID, InstanceID, TargetRoleID, 	8990113	, 1, -1, 3, 104)
	end
end

--离婚通用检查
function Marriage_BreakMarriageCommonCheck(ApplicantRoleID, TargetRoleID)

	--性别是否相同
	if role.GetRoleSex(ApplicantRoleID) == role.GetRoleSex(TargetRoleID) then
		return 2 --性别相同
	end
	
	--检查申请者是否已婚
	if role.IsRoleMarried(ApplicantRoleID) == false then 
		return 11 --申请者未婚
	end

	return 0 --成功

end

--离婚
function Marriage_BreakMarriageEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--删除结婚戒指
	Marriage_DelRing(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--减申请者的钱钱
	role.DecRoleSilver(MapID, InstanceID, ApplicantRoleID, marriage_BreakMarriageDecSilver, 101)

end

--离婚检查
function Marriage_BreakMarriageCheckEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--离婚通用检查
	CommonCheckResult = Marriage_BreakMarriageCommonCheck(ApplicantRoleID, TargetRoleID)
	if CommonCheckResult ~= 0 then
		return CommonCheckResult
	end

	--申请者金钱是否足够
	if role.GetRoleSilver(MapID,InstanceID,ApplicantRoleID) < marriage_BreakMarriageDecSilver then
		return 7 --申请者必须拥有总够的离婚费用
	end

	return 0 --成功
end

--强制离婚
function Marriage_ForceBreakMarriageEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--删除结婚戒指
	Marriage_DelRing(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--从申请者背包内扣除绝情丹,typeid=1360118
	role.RemoveFromRole(MapID, InstanceID, ApplicantRoleID, 1360118, 1, 101);

end

--强制离婚检查
function Marriage_ForceBreakMarriageCheckEvent(MapID, InstanceID, ApplicantRoleID, TargetRoleID)

	--离婚通用检查
	CommonCheckResult = Marriage_BreakMarriageCommonCheck(ApplicantRoleID, TargetRoleID)
	if CommonCheckResult ~= 0 then
		return CommonCheckResult
	end

	--检查角色身上是否有绝情丹，typeid=1360118
	if role.GetRoleItemNum(ApplicantRoleID, 1360118) <= 0 then
		return 13
	end

	return 0 --成功
end

aux.RegisterRoleEvent(11, "Marriage_GetMarriageEvent")
aux.RegisterRoleEvent(12, "Marriage_GetMarriageCheckEvent")

aux.RegisterRoleEvent(13, "Marriage_BreakMarriageEvent")
aux.RegisterRoleEvent(14, "Marriage_BreakMarriageCheckEvent")

aux.RegisterRoleEvent(42, "Marriage_ForceBreakMarriageEvent")
aux.RegisterRoleEvent(43, "Marriage_ForceBreakMarriageCheckEvent")
