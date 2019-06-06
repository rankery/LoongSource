
--地图ScriptData
--m04索引3 ，轩辕神龙 召唤绿晶球 消失数量
--m04索引4 ，轩辕神龙 召唤紫晶球 消失数量

--BOSS
--1550201	女娲神龙
--1550202	轩辕神龙
--1550203	三苗神龙
--1550204	九黎神龙

--龙之穴BOSS技能

--2429701	哀鸿遍野
--2429801	百转轮回
--2429901	山崩地裂
--2430001	石破天惊

---女娲神龙
--1550205	生之晶球
--1550206	灭之晶球

--记录轩辕神龙ID
xuanyuanshenlong = 0

function s2429701_Cast(MapID, InstanceID, SkillID, OwnerID)

local localA, localB ,localC = unit.GetPosition(MapID, InstanceID, OwnerID)
local position={}
position[1]={localA+10, localB, localC+20}
position[2]={localA+10, localB, localC+18}
position[3]={localA+10, localB, localC+15}
position[4]={localA+10, localB, localC+12}
position[5]={localA+10, localB, localC+10}
position[6]={localA+10, localB, localC-10}
position[7]={localA+10, localB, localC-12}
position[8]={localA+10, localB, localC-15}
position[9]={localA+10, localB, localC-18}
position[10]={localA+10, localB, localC-20}

position[11]={localA-10, localB, localC+20}
position[12]={localA-10, localB, localC+18}
position[13]={localA-10, localB, localC+15}
position[14]={localA-10, localB, localC+12}
position[15]={localA-10, localB, localC+10}
position[16]={localA-10, localB, localC-10}
position[17]={localA-10, localB, localC-12}
position[18]={localA-10, localB, localC-15}
position[19]={localA-10, localB, localC-18}
position[20]={localA-10, localB, localC-20}

	local m = math.random(1,10)
	local n = math.random(11,20)
	map.MapCreateCreature(MapID, InstanceID, 1550205, position[m][1], position[m][2], position[m][3])
	map.MapCreateCreature(MapID, InstanceID, 1550206, position[n][1], position[n][2], position[n][3])

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3801)  --女娲神龙释放了大范围毒雾，并进入无敌状态。利用生之晶球和灭之晶球可以帮助大家渡过难关。
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

--注册

aux.RegisterSkillEvent(2429701, 1, "s2429701_Cast")

--9435201	·触发取消无敌

function c1550206_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

unit.AddBuff(MapID, InstanceID,RoleID,9435201,RoleID)
end
--注册
aux.RegisterCreatureEvent(1550206, 4, "c1550206_OnDie")


---轩辕神龙

--[[
1550207	绿晶球
1550208	紫晶球

9435501				瞬间恢复10%体力
9435601				瞬间恢复20%体力
9435701				瞬间恢复30%体力
9435801				瞬间恢复40%体力
9435901				瞬间恢复50%体力
9436001				紫晶石之力
9436101				紫晶石之力
9436201				紫晶石之力
9436301				紫晶石之力
9436401				紫晶石之力]]

function s2429801_Cast(MapID, InstanceID, SkillID, OwnerID)

xuanyuanshenlong = OwnerID

local localA, localB ,localC = unit.GetPosition(MapID, InstanceID, OwnerID)
local position={}
position[1]={x=1133, y=13475, z=1873}
position[2]={x=1138, y=13463, z=1846}
position[3]={x=1138, y=13481, z=1856}
position[4]={x=1138, y=13434, z=1831}
position[5]={x=1150, y=13393, z=1868}
position[6]={x=1165, y=13184, z=1882}
position[7]={x=1166, y=13234, z=1869}
position[8]={x=1162, y=13302, z=1853}
position[9]={x=1148, y=13423, z=1864}
position[10]={x=1158, y=13334, z=1863}

position[11]={x=1135, y=13437, z=1823}
position[12]={x=1127, y=13453, z=1814}
position[13]={x=1115, y=13459, z=1814}
position[14]={x=1137, y=13428, z=1823}
position[15]={x=1144, y=13368, z=1817}
position[16]={x=1132, y=13450, z=1831}
position[17]={x=1126, y=13464, z=1838}
position[18]={x=1136, y=13470, z=1847}
position[19]={x=1132, y=13452, z=1833}
position[20]={x=1155, y=13294, z=1878}


local a = math.random(1,2)
	map.MapCreateCreature(MapID, InstanceID, 1550207, position[a].x, position[a].y, position[a].z)

local b = math.random(3,4)
	map.MapCreateCreature(MapID, InstanceID, 1550208, position[b].x, position[b].y, position[b].z)

local c = math.random(5,6)
	map.MapCreateCreature(MapID, InstanceID, 1550207, position[c].x, position[c].y, position[c].z)

local d = math.random(7,8)
	map.MapCreateCreature(MapID, InstanceID, 1550208, position[d].x, position[d].y, position[d].z)

local e = math.random(9,10)
	map.MapCreateCreature(MapID, InstanceID, 1550207, position[e].x, position[e].y, position[e].z)

local f = math.random(11,12)
	map.MapCreateCreature(MapID, InstanceID, 1550208, position[f].x, position[f].y, position[f].z)

local g = math.random(13,14)
	map.MapCreateCreature(MapID, InstanceID, 1550207, position[g].x, position[g].y, position[g].z)

local h = math.random(15,16)
	map.MapCreateCreature(MapID, InstanceID, 1550208, position[h].x, position[h].y, position[h].z)

local i = math.random(17,18)
	map.MapCreateCreature(MapID, InstanceID, 1550207, position[i].x, position[i].y, position[i].z)

local j = math.random(19,20)
	map.MapCreateCreature(MapID, InstanceID, 1550208, position[j].x, position[j].y, position[j].z)

	--计时用怪物
	map.MapCreateCreature(MapID, InstanceID, 1550210, localA, localB ,localC)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3802)    --轩辕神龙已经召唤出了绿晶球和紫晶球，迅速减少晶球数量是制胜的关键。
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterSkillEvent(2429801, 1, "s2429801_Cast")

--绿晶球死亡

function c1550207_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local m = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	m = m + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 3, m)
end

aux.RegisterCreatureEvent(1550207, 4, "c1550207_OnDie")



--紫晶球死亡

function c1550208_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local n = map.GetMapScriptData(MapID, InstanceID, 1, 4)
	n = n + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 4, n)
end

aux.RegisterCreatureEvent(1550208, 4, "c1550208_OnDie")


---隐形计时器消失

function c1550210_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local m = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	local n = map.GetMapScriptData(MapID, InstanceID, 1, 4)
	if m == 0 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9435901,xuanyuanshenlong)
	elseif m == 1 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9435801,xuanyuanshenlong)
	elseif m == 2 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9435701,xuanyuanshenlong)
	elseif m == 3 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9435601,xuanyuanshenlong)
	elseif m == 4 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9435501,xuanyuanshenlong)
	end
	if n == 0 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9436401,xuanyuanshenlong)
	elseif n == 1 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9436301,xuanyuanshenlong)
	elseif n == 2 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9436201,xuanyuanshenlong)
	elseif n == 3 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9436101,xuanyuanshenlong)
	elseif n == 4 then
		unit.AddBuff(MapID, InstanceID,xuanyuanshenlong,9436001,xuanyuanshenlong)
	end

	map.SetMapScriptData(MapID, InstanceID, 1, 3, 0)
	map.SetMapScriptData(MapID, InstanceID, 1, 4, 0)
end

aux.RegisterCreatureEvent(1550210, 13, "c1550210_OnDisappear")


function c1550201_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3625)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchWorldMsgEvent(MsgID)

	role.SigTitleEvent(RoleID, 285)

end

aux.RegisterCreatureEvent(1550201, 4, "c1550201_OnDie")

function c1550202_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	xuanyuanshenlong = 0

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3625)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchWorldMsgEvent(MsgID)

	role.SigTitleEvent(RoleID, 285)

end

aux.RegisterCreatureEvent(1550202, 4, "c1550202_OnDie")

function c1550203_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3625)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchWorldMsgEvent(MsgID)

	role.SigTitleEvent(RoleID, 285)

end

aux.RegisterCreatureEvent(1550203, 4, "c1550203_OnDie")

function c1550204_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3625)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchWorldMsgEvent(MsgID)

	role.SigTitleEvent(RoleID, 285)

end

aux.RegisterCreatureEvent(1550204, 4, "c1550204_OnDie")


--三苗神龙

--1550209	剧毒蘑菇（出生自带BUFF）


function s2429901_Cast(MapID, InstanceID, SkillID, OwnerID)

local localA, localB ,localC = unit.GetPosition(MapID, InstanceID, OwnerID)
local position={}
position[1]={localA+10, localB, localC+20}
position[2]={localA+12, localB, localC+18}
position[3]={localA+15, localB, localC+15}
position[4]={localA+18, localB, localC+12}
position[5]={localA+10, localB, localC+10}
position[6]={localA+12, localB, localC-10}
position[7]={localA+15, localB, localC-12}
position[8]={localA+18, localB, localC-15}
position[9]={localA+10, localB, localC-18}
position[10]={localA+12, localB, localC-20}

position[11]={localA-12, localB, localC+20}
position[12]={localA-15, localB, localC+18}
position[13]={localA-18, localB, localC+15}
position[14]={localA-10, localB, localC+12}
position[15]={localA-12, localB, localC+10}
position[16]={localA-15, localB, localC-10}
position[17]={localA-18, localB, localC-12}
position[18]={localA-10, localB, localC-15}
position[19]={localA-12, localB, localC-18}
position[20]={localA-15, localB, localC-20}

local a = math.random(1,3)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[a][1], position[a][2], position[a][3])

local b = math.random(4,6)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[b][1], position[b][2], position[b][3])

local c = math.random(7,10)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[c][1], position[c][2], position[c][3])

local d = math.random(11,13)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[d][1], position[d][2], position[d][3])

local e = math.random(14,16)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[e][1], position[e][2], position[e][3])

local f = math.random(17,20)
	map.MapCreateCreature(MapID, InstanceID, 1550209, position[f][1], position[f][2], position[f][3])

	--local Sec  = tonumber(os.date("%S"))
	dragon_timer = 50
	dragon_timerbegin = 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3805)    --剧毒蘑菇已经被召唤出来，X秒后蘑菇会对附近的单位造成巨大伤害。
	msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)
	--SystemActivity.lua 公告每隔1秒发布一次
end

aux.RegisterSkillEvent(2429901, 1, "s2429901_Cast")

--九黎神龙

function s2430001_Cast(MapID, InstanceID, SkillID, OwnerID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3804)    --九黎神龙释放出神力，令自己进入无敌状态，并提高了附近单位的伤害。&lt;color=0xffff0000&gt;“开始疯狂的杀戮吧！胜者，才配和老夫交手……”&lt;color=0xfffff7e0&gt;
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

aux.RegisterSkillEvent(2430001, 1, "s2430001_Cast")




































