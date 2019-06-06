--[[
1350019	星辰符
8971001	七杀
8971002	七杀
8971003	七杀
8971004	七杀
8971005	七杀
8971006	七杀
8971007	七杀
8971008	破军
8971009	破军
8971010	破军
8971011	破军
8971012	破军
8971013	贪狼
8971014	贪狼
8971015	贪狼]]



--可否使用函数
function i1350019_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	for i = 8971001, 8971015 do
		if 1 <= role.GetRoleItemNum(TargetID, i) then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

aux.RegisterItemEvent(1350019, 0, "i1350019_CanUse")
aux.RegisterItemEvent(1350026, 0, "i1350019_CanUse")

--对戒指进行升级
function I1350019_OnUse(MapID, InstanceID, TypeID, TargetID)

local wabao_jiezhi={}
wabao_jiezhi[8971001]={lv=40,Index=1}
wabao_jiezhi[8971002]={lv=50,Index=1}
wabao_jiezhi[8971003]={lv=60,Index=1}
wabao_jiezhi[8971004]={lv=70,Index=1}
wabao_jiezhi[8971005]={lv=80,Index=1}
wabao_jiezhi[8971006]={lv=90,Index=1}
wabao_jiezhi[8971007]={lv=100,Index=1}
wabao_jiezhi[8971008]={lv=60,Index=2}
wabao_jiezhi[8971009]={lv=70,Index=2}
wabao_jiezhi[8971010]={lv=80,Index=2}
wabao_jiezhi[8971011]={lv=90,Index=2}
wabao_jiezhi[8971012]={lv=100,Index=2}
wabao_jiezhi[8971013]={lv=80,Index=3}
wabao_jiezhi[8971014]={lv=90,Index=3}
wabao_jiezhi[8971015]={lv=100,Index=3}

	for i = 8971001, 8971015 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			if  wabao_jiezhi[i].lv == 100 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 5, 8, 420)
			elseif wabao_jiezhi[i].lv == 90 then
					local temp=math.random(1,100)
					if temp <= 5 then	--升级	使用了一个星辰符后，成功将&lt;p2&gt;升级成了&lt;p3&gt;级，真是羡煞旁人！
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 5, 8, 420)
					elseif temp>=6 and temp<=20 then  -- 重铸  &lt;p1&gt;使用了一个星辰符，将&lt;p2&gt;的属性进行了重铸！
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					else --等级降低  &lt;p1&gt;使用了一个星辰符，但&lt;p2&gt;的等级降低了10级，真是可惜啊！
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 424)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i-1, 1, 4, 8, 420)
					end
			elseif wabao_jiezhi[i].lv ==80 then
				local temp=math.random(1,100)
					if temp<= 10 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 4, 8, 420)
					elseif temp>= 11 and temp<= 30 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					else
						if wabao_jiezhi[i].Index == 3 then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 421)
							msg.AddMsgEvent(MsgID, 2, TargetID)
							msg.AddMsgEvent(MsgID, 4, i)
							msg.DispatchRoleMsgEvent(TargetID, MsgID)
							role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
						else
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 424)
							msg.AddMsgEvent(MsgID, 2, TargetID)
							msg.AddMsgEvent(MsgID, 4, i)
							msg.DispatchRoleMsgEvent(TargetID, MsgID)
							role.AddRoleItem(MapID, InstanceID, TargetID, i-1, 1, 4, 8, 420)
						end
					end
			elseif wabao_jiezhi[i].lv == 70 then
					local temp=math.random(1,100)
					if temp<= 15 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 4, 8, 420)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					end
			elseif  wabao_jiezhi[i].lv == 60 then
				local temp=math.random(1,100)
					if temp<= 10 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 4, 8, 420)
					elseif temp>= 11 and temp<= 40 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					else
						if wabao_jiezhi[i].Index == 2 then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 421)
							msg.AddMsgEvent(MsgID, 2, TargetID)
							msg.AddMsgEvent(MsgID, 4, i)
							msg.DispatchRoleMsgEvent(TargetID, MsgID)
							role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
						else
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 424)
							msg.AddMsgEvent(MsgID, 2, TargetID)
							msg.AddMsgEvent(MsgID, 4, i)
							msg.DispatchRoleMsgEvent(TargetID, MsgID)
							role.AddRoleItem(MapID, InstanceID, TargetID, i-1, 1, 4, 8, 420)
						end
					end
			elseif wabao_jiezhi[i].lv == 50 then
				local temp=math.random(1,100)
					if temp<= 15 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 4, 8, 420)
					elseif temp>= 16 and temp<= 40 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 424)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i-1, 1, 4, 8, 420)
					end
			elseif wabao_jiezhi[i].lv == 40 then
				local temp=math.random(1,100)
					if temp<= 50 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 100, 420)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i+1)
						msg.AddMsgEvent(MsgID, 9, wabao_jiezhi[i+1].lv)
						msg.DispatchWorldMsgEvent(MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i+1, 1, 4, 8, 420)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 0)
						msg.AddMsgEvent(MsgID, 1, 421)
						msg.AddMsgEvent(MsgID, 2, TargetID)
						msg.AddMsgEvent(MsgID, 4, i)
						msg.DispatchRoleMsgEvent(TargetID, MsgID)
						role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, 4, 8, 420)
					end
			end
			return
		end
	end
end

aux.RegisterItemEvent(1350019, 1, "I1350019_OnUse")
aux.RegisterItemEvent(1350026, 1, "I1350019_OnUse")
