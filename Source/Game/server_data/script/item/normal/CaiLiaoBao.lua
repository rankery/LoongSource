--相关材料ID
--[[
1330007	精良矿石包
1330008	精良布料包
1330009	上等药材包*
1330010	上等食材包*
1330011	优质矿石包
1330012	优质布料包
1330013	优质药材包*
1330014	优质食材包*
1330015	稀有矿石包
1330016	稀有布料包
1330017	珍稀药材包*
1330018	珍稀食材包*
]]

Cailiaobao={}

Cailiaobao[1330007]={item1=2000002,item2=2000007,item3=2000012,item4=2000017,item5=2000022,num=8,sp=1}
Cailiaobao[1330008]={item1=2000202,item2=2000207,item3=2000212,item4=2000217,item5=2000222,num=8,sp=1}
Cailiaobao[1330009]={item1=2000102,item2=2000107,item3=2000112,item4=2000117,item5=2000122,num=8,sp=2}
Cailiaobao[1330010]={item1=2000302,item2=2000307,item3=2000312,item4=2000317,item5=2000322,num=8,sp=2}
Cailiaobao[1330011]={item1=2000003,item2=2000008,item3=2000013,item4=2000018,item5=2000023,num=4,sp=1}
Cailiaobao[1330012]={item1=2000203,item2=2000208,item3=2000213,item4=2000218,item5=2000223,num=4,sp=1}
Cailiaobao[1330013]={item1=2000103,item2=2000108,item3=2000113,item4=2000118,item5=2000123,num=4,sp=2}
Cailiaobao[1330014]={item1=2000303,item2=2000308,item3=2000313,item4=2000318,item5=2000323,num=4,sp=2}
Cailiaobao[1330015]={item1=2000004,item2=2000009,item3=2000014,item4=2000019,item5=2000024,num=2,sp=1}
Cailiaobao[1330016]={item1=2000204,item2=2000209,item3=2000214,item4=2000219,item5=2000224,num=2,sp=1}
Cailiaobao[1330017]={item1=2000104,item2=2000109,item3=2000114,item4=2000119,item5=2000124,num=2,sp=2}
Cailiaobao[1330018]={item1=2000304,item2=2000309,item3=2000314,item4=2000319,item5=2000324,num=2,sp=2}

function Cailiaobao_OnUse(MapID, InstanceID, TypeID, RoleID)
		local a = Cailiaobao[TypeID].sp
		local b = Cailiaobao[TypeID].item1
		local c = Cailiaobao[TypeID].item2
		local d = Cailiaobao[TypeID].item3
		local e = Cailiaobao[TypeID].item4
		local f = Cailiaobao[TypeID].item5
		local n = Cailiaobao[TypeID].num
		if a == 1 then
			local k = math.random(1,100)
				if  k>=1 and k <=30 then
					role.AddRoleItem(MapID, InstanceID, RoleID, b, n, -1, 8, 420)
				elseif k>=31 and k <=60 then
					role.AddRoleItem(MapID, InstanceID, RoleID, c, n, -1, 8, 420)
				elseif k>=61 and k <=80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, d, n, -1, 8, 420)
				elseif k>=81 and k <=90 then
					role.AddRoleItem(MapID, InstanceID, RoleID, e, n, -1, 8, 420)
				elseif k>=91 and k <=100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, f, n, -1, 8, 420)
				end
		else
			local t = math.random(1,100)
				if  t>=1 and t <=20 then
					role.AddRoleItem(MapID, InstanceID, RoleID, b, n, -1, 8, 420)
				elseif t>=21 and t <=40 then
					role.AddRoleItem(MapID, InstanceID, RoleID, c, n, -1, 8, 420)
				elseif t>=41 and t <=60 then
					role.AddRoleItem(MapID, InstanceID, RoleID, d, n, -1, 8, 420)
				elseif t>=61 and t <=80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, e, n, -1, 8, 420)
				elseif t>=81 and t <=100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, f, n, -1, 8, 420)
				end
		end

end

aux.RegisterItemEvent(1330007, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330008, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330011, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330012, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330015, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330016, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330009, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330010, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330013, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330014, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330017, 1, "Cailiaobao_OnUse")
aux.RegisterItemEvent(1330018, 1, "Cailiaobao_OnUse")


function Cailiaobao_CanUse(MapID, InstanceID, TypeID, RoleID)
		local bRet, bIgnore = 0, false
		--判断背包空闲空间是否足够
		local FreeSize = role.GetBagFreeSize(RoleID)
		if(FreeSize < 1) then
			--提示玩家背包空间不足
			bRet = 40
			return bRet, bIgnore
		end
end

aux.RegisterItemEvent(1330007, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330008, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330009, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330010, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330011, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330012, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330013, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330014, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330015, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330016, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330017, 0, "Cailiaobao_CanUse")
aux.RegisterItemEvent(1330018, 0, "Cailiaobao_CanUse")




