--七夕活动 坐标
qixi_map_m04 = {}
qixi_map_m04[1]={x=1300,y=13342,z=1234}
qixi_map_m04[2]={x=1123,y=13000,z=816}
qixi_map_m04[3]={x=1626,y=14330,z=678}
qixi_map_m04[4]={x=2369,y=12733,z=974}
qixi_map_m04[5]={x=1966,y=13038,z=1463}
qixi_map_m04[6]={x=2395,y=13118,z=1509}
qixi_map_m04[7]={x=3009,y=21794,z=1808}
qixi_map_m04[8]={x=2439,y=12980,z=2128}
qixi_map_m04[9]={x=2465,y=20840,z=3055}
qixi_map_m04[10]={x=1826,y=12520,z=2778}
qixi_map_m04[11]={x=1645,y=12575,z=2308}
qixi_map_m04[12]={x=1612,y=12702,z=1922}
qixi_map_m04[13]={x=1073,y=12966,z=1927}
qixi_map_m04[14]={x=1277,y=13156,z=1492}
qixi_map_m04[15]={x=1286,y=13243,z=1241}
qixi_map_m04[16]={x=1732,y=13243,z=1197}
qixi_map_m04[17]={x=2802,y=12317,z=1114}
qixi_map_m04[18]={x=1881,y=13085,z=1662}
qixi_map_m04[19]={x=1779,y=12575,z=2364}
qixi_map_m04[20]={x=2718,y=20883,z=3350}


qixi_map_m02 = {}
qixi_map_m02[1]={x=2311,y=3879,z=1125}
qixi_map_m02[2]={x=3342,y=4635,z=1042}
qixi_map_m02[3]={x=3266,y=3924,z=853}
qixi_map_m02[4]={x=3680,y=3887,z=797}
qixi_map_m02[5]={x=3284,y=3661,z=549}
qixi_map_m02[6]={x=3676,y=3637,z=1890}
qixi_map_m02[7]={x=3511,y=3648,z=2014}
qixi_map_m02[8]={x=3127,y=4188,z=2157}
qixi_map_m02[9]={x=2985,y=3642,z=2340}
qixi_map_m02[10]={x=3100,y=6930,z=2540}
qixi_map_m02[11]={x=2779,y=6282,z=2562}
qixi_map_m02[12]={x=3359,y=5667,z=2973}
qixi_map_m02[13]={x=3130,y=6075,z=3079}
qixi_map_m02[14]={x=3408,y=6719,z=3752}
qixi_map_m02[15]={x=3125,y=7550,z=3628}
qixi_map_m02[16]={x=2710,y=6989,z=3234}
qixi_map_m02[17]={x=1725,y=8177,z=3294}
qixi_map_m02[18]={x=1472,y=8002,z=3076}
qixi_map_m02[19]={x=1015,y=7823,z=3642}
qixi_map_m02[20]={x=1699,y=3995,z=2113}


m02_qixi_point={}
m02_qixi_point[1]={mapcrc=3017298127, x=2296, y=5756, z=2204, npc=0}
m02_qixi_point[2]={mapcrc=3017298127, x=3265, y=4885, z=1597, npc=0}
m02_qixi_point[3]={mapcrc=3017298127, x=3184, y=8390, z=3356, npc=0}
m02_qixi_point[4]={mapcrc=3017298127, x=1925, y=8000, z=3624, npc=0}

m04_qixi_point={}
m04_qixi_point[1]={mapcrc=3017299663, x=2074, y=18342, z=2425, npc=0}
m04_qixi_point[2]={mapcrc=3017299663, x=1099, y=13199, z=1683, npc=0}
m04_qixi_point[3]={mapcrc=3017299663, x=3053, y=23613, z=1462, npc=0}
m04_qixi_point[4]={mapcrc=3017299663, x=2745, y=13900, z=800, npc=0}

--[[jinge1 = 0
yumei1 = 0
jinge2 = 0
yumei2 = 0]]

function QiXi_OnStart(actID)

    if act.GetActScriptData(99, 1, 42) == 0 then   --清空排名数据
	    for i=0, 29 do
		    act.SetActScriptData(151, 1, i, 0)
		end
        act.SetActScriptData(99, 1, 42, 1)
		act.SaveActScriptData(99)
		act.SaveActScriptData(151)
	end

	local k = act.GetActScriptData(151, 1, 30)
	local curyear = tonumber(os.date("%Y"))
	if k == 0 or k == nil or k ~= curyear then
	    for i=0, 29 do
		    act.SetActScriptData(151, 1, i, 0)
		end
		act.SetActScriptData(151, 1, 30, curyear)
		act.SaveActScriptData(151)
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100138)  --七夕是中国的传统情人节，愿所有的的有情人都能终成眷属！七夕活动的相关介绍请在龙城和黄帝城的金哥处进行查询！
	msg.DispatchBroadcast(MsgID,-1,-1,-1)

	--[[jinge1 = map.MapCreateCreature(3017298127, -1, , , 6646, )
	--yumei1 = map.MapCreateCreature(3017298127, -1, , , 6646, )
	--jinge2 = map.MapCreateCreature(3017299663, -1, , , 18345, )
	--yumei2 = map.MapCreateCreature(3017299663, -1, , , 18345, )
	for randindex=1,4 do
		m02_qixi_point[randindex].npc = 0
		m04_qixi_point[randindex].npc = 0
	end

	local a = math.random(4)
	local b = math.random(4)
	local c = 4

	while (b == a and b > 0) do
		b = c
		c = c - 1
	end

	map.MapCreateColCreature(3017298127, -1, 4900492, m02_qixi_point[a].x, m02_qixi_point[a].y, m02_qixi_point[a].z, 1)
	m02_qixi_point[a].npc = 1                   -- 牛郎
	map.MapCreateColCreature(3017298127, -1, 4900492, m04_qixi_point[a].x, m04_qixi_point[a].y, m04_qixi_point[a].z, 1)
	m04_qixi_point[a].npc = 1                   -- 牛郎
	map.MapCreateColCreature(3017299663, -1, 4900493, m02_qixi_point[b].x, m02_qixi_point[b].y, m02_qixi_point[b].z, 1)
	m02_qixi_point[a].npc = 2                   -- 织女
	map.MapCreateColCreature(3017299663, -1, 4900493, m04_qixi_point[b].x, m04_qixi_point[b].y, m04_qixi_point[b].z, 1)
	m04_qixi_point[a].npc = 2                   -- 织女

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, )  --下凡牛郎和下凡织女已经出现在了龙城和涿鹿，您可以去找他们兑换姻缘结。
	msg.DispatchWorldMsgEvent(MsgID)

	for randindex=1,20 do
		map.MapCreateColCreature(3017298127, -1, 1534144, qixi_map_m04[randindex].x, qixi_map_m04[randindex].y, qixi_map_m04[randindex].z, 1)
		map.MapCreateColCreature(3017299663, -1, 1534145, qixi_map_m02[randindex].x, qixi_map_m02[randindex].y, qixi_map_m02[randindex].z, 1)
	end

	if curmin == 30 or curmin == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, )  --七夕活动期间每10分钟都会刷出奉旨天兵，击杀后可以获得放牛绳和金银梭，
		msg.DispatchWorldMsgEvent(MsgID)
	end]]

end

function QiXi_OnTimerMin(actID)

	local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))

	if (curhour>= 19  and curhour<= 22 ) and curmin % 10==0 then

	    for randindex=1,4 do
            m02_qixi_point[randindex].npc = 0
			m04_qixi_point[randindex].npc = 0
		end

		local a = math.random(4)
		local b = math.random(4)
		local c = 4

		while (b == a and b > 0) do
		    b = c
			c = c - 1
			a = c
		end

		map.MapCreateColCreature(3017298127, -1, 4900492, m02_qixi_point[a].x, m02_qixi_point[a].y, m02_qixi_point[a].z, 1)
		m02_qixi_point[a].npc = 1                   -- 牛郎
		map.MapCreateColCreature(3017299663, -1, 4900492, m04_qixi_point[a].x, m04_qixi_point[a].y, m04_qixi_point[a].z, 1)
		m04_qixi_point[a].npc = 1                   -- 牛郎
		map.MapCreateColCreature(3017298127, -1, 4900493, m02_qixi_point[b].x, m02_qixi_point[b].y, m02_qixi_point[b].z, 1)
		m02_qixi_point[b].npc = 2                   -- 织女
		map.MapCreateColCreature(3017299663, -1, 4900493, m04_qixi_point[b].x, m04_qixi_point[b].y, m04_qixi_point[b].z, 1)
		m04_qixi_point[b].npc = 2                   -- 织女

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, 12101)  --下凡牛郎和下凡织女已经出现在了龙城和涿鹿，您可以去找他们兑换姻缘结。
		msg.DispatchWorldMsgEvent(MsgID)

		for randindex=1,20 do
            map.MapCreateColCreature(3017299663, -1, 1534144, qixi_map_m04[randindex].x, qixi_map_m04[randindex].y, qixi_map_m04[randindex].z, 1)
		    map.MapCreateColCreature(3017298127, -1, 1534145, qixi_map_m02[randindex].x, qixi_map_m02[randindex].y, qixi_map_m02[randindex].z, 1)
        end

		if curmin == 30 or curmin == 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 12102)  --七夕活动期间每10分钟都会刷出奉旨天兵，击杀后可以获得放牛绳和金银梭，
			msg.DispatchWorldMsgEvent(MsgID)
	    end

	end

	if curhour == 23 and curmin == 0 then
		m02_qixi_point[1].npc = 0
		m02_qixi_point[2].npc = 0
		m02_qixi_point[3].npc = 0
		m02_qixi_point[4].npc = 0
		m04_qixi_point[1].npc = 0
		m04_qixi_point[2].npc = 0
		m04_qixi_point[3].npc = 0
		m04_qixi_point[4].npc = 0
	end

end

--[[npc生存时间到，活动结束，在各城市中删除此npc
function QiXi_OnEnd(actID)
	map.MapDeleteCreature(3017298127, -1, jinge1)
	map.MapDeleteCreature(3017298127, -1, yumei1)
	map.MapDeleteCreature(3017299663, -1, jinge2)
	map.MapDeleteCreature(3017299663, -1, yumei2)
end]]

--注册该活动的相关事件
aux.RegisterActEvent(151, 2, "QiXi_OnStart")
--aux.RegisterActEvent(, 3, "QiXi_OnEnd")
aux.RegisterActEvent(151, 4, "QiXi_OnTimerMin")

--与金哥对话
function n4900490_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
	    local bStart = act.GetActIsStart(151)
	    if bStart then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12103) -- "金哥：\n        迢迢牵牛星，皎皎河汉女。\n        纤纤擢素手，札扎弄机杼。\n      终日不成章，泣涕零如雨。\n        河汉清且浅，相去复几许。\n        盈盈一水间，脉脉不得语。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 12104) -- “兑换姻缘结排行榜（前十名）”
			msg.AddMsgEvent(MsgID, 21, 4)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 12105)   --"七夕的由来"
			msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 12106)   --"七夕活动说明"
			msg.AddMsgEvent(MsgID, 21, 7)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 12107)   --"七夕活动的最终奖励"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12103) -- "金哥：\n        迢迢牵牛星，皎皎河汉女。\n        纤纤擢素手，札扎弄机杼。\n      终日不成章，泣涕零如雨。\n        河汉清且浅，相去复几许。\n        盈盈一水间，脉脉不得语。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 12104) -- “兑换姻缘结排行榜（前十名）”
			msg.AddMsgEvent(MsgID, 21, 8)   --领取奖励
			msg.AddMsgEvent(MsgID, 1, 12108)   --"领取最终奖励"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
    elseif TalkIndex == 4 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12109) -- "金哥：\n        听爸爸跟我们说，他以前只是一个普普通通的农家放牛娃，大伯和婶婶对他很不好。后来有一天，与他朝夕相伴的老黄牛居然开口说话了，告诉他七月七日那天，天上的仙女将会下凡戏水，只要能把仙女的衣裳偷偷的藏起来，他就能让仙女成为自己的妻子。爸爸照黄牛的话做了，妈妈本来又羞又气，但是看到爸爸那憨厚的笑脸，终于心软下来，成为了他的妻子。这么一来，才有了我和玉妹！不过王母娘娘对于妈妈私许凡人的做法非常不满，她让天兵将妈妈抓走，还用发钗在天上划了一条银河，阻断了我们一家人相见的天路，并规定只有每年的七月七日，我们才能在鹊桥上相见一次……"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	elseif TalkIndex == 6 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12110) -- "金哥：\n        七夕的活动时间是：8月16日至8月30日。\n        七夕的活动内容：七夕活动期间，每天晚上19点至23点，每隔10分钟就会在龙城和涿鹿的场景内出现下凡牛郎、下凡织女或者奉旨天兵。在牛郎处可以用金银梭来兑换姻缘结，在织女处可以用放牛绳兑换姻缘结。奉旨天兵则可以被攻击，打败它后可以获得金银梭、放牛绳及其他珍贵道具。在牛郎和织女身边使用交互动作“拥抱”和“接吻”，可以获得一枚（接吻为两枚）金银梭或者放牛绳。另外，金银梭和放牛绳还可以在活动期间在副本最终BOSS身上获得。\n        您可以在玉妹处使用姻缘结兑换各种物品奖励。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 7 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12111) -- "金哥：\n        可选奖励：称号：相约鹊桥，法：王母发钗。\n        兑换姻缘结次数最多的第1-3名玩家可以获得这两件道具，而之后的7名玩家只能选取其中的1件。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 5 then
	    local k = nil
		for i = 0, 9 do
		    if act.GetActScriptData(151, 1, i) ~= nil and act.GetActScriptData(151, 1, i) > 0 then
			    k = i
			end
		end
		if k == nil then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12112) -- "金哥：\n        每天晚上七点到十一点，您可以在下凡牛郎和下凡织女处兑换姻缘结。现在还没有人兑换过姻缘结！"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12113+k) -- "金哥：\n        现在兑换姻缘结数量最多的是：\n        XXX        XXX个姻缘结
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			for i = 0,k do
				msg.AddMsgEvent(MsgID, 9, act.GetActScriptData(151, 1, i+10))
				msg.AddMsgEvent(MsgID, 2, act.GetActScriptData(151, 1, i))
			end
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	elseif TalkIndex == 8 then
	    local k = nil
		for i = 0, 9 do
		    if act.GetActScriptData(151, 1, i) ~= nil and act.GetActScriptData(151, 1, i) > 0 and act.GetActScriptData(151, 1, i) == RoleID then
			    k = i
				break
			end
		end
		if k == nil or act.GetActIsStart(151) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12127) -- "金哥：\n        很抱歉，您没有进入排行榜。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif act.GetActScriptData(151, 1, k+20) == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12128) -- "金哥：\n        对不起，您已经领取过奖励了。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
		    if k >= 0 and k <= 2 then
			    if role.GetBagFreeSize(RoleID) > 1 then
				    local attval1=role.GetRoleAttValue(MapID, InstanceID, RoleID, 1)
					local attval2=role.GetRoleAttValue(MapID, InstanceID, RoleID, 3)
					if attval1 > attval2 then
						role.AddRoleItem(MapID, InstanceID, RoleID, 8600099, 1, 5, 13, 1001)  --法宝
					else
						role.AddRoleItem(MapID, InstanceID, RoleID, 8600098, 1, 5, 13, 1001)  --法宝
					end
					role.AddRoleItem(MapID, InstanceID, RoleID, 3305007, 1, -1, 13, 1001)  --称号
					act.SetActScriptData(151, 1, k+20, 1)
					act.SaveActScriptData(151)
				else
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 12123) -- "金哥：\n        请您保证自己的行囊中有足够的空间再来领取。"
					msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
					msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12124) -- "金哥：\n        请选择您想要领取的奖励。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, 9) -- 领取法宝：王母发钗
				msg.AddMsgEvent(MsgID, 1, 12125) -- “确定”
				msg.AddMsgEvent(MsgID, 21, 10) -- 领取称号：相约鹊桥
				msg.AddMsgEvent(MsgID, 1, 12126) -- “确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		end
	elseif TalkIndex == 9 then
	    local k = nil
		for i = 0, 9 do
		    if act.GetActScriptData(151, 1, i) ~= nil and act.GetActScriptData(151, 1, i) > 0 and act.GetActScriptData(151, 1, i) == RoleID then
			    k = i
				break
			end
		end
		if k == nil then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12127) -- "金哥：\n        很抱歉，您没有进入排行榜。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif act.GetActScriptData(151, 1, k+20) == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12128) -- "金哥：\n        对不起，您已经领取过奖励了。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
			if role.GetBagFreeSize(RoleID) > 0 then
			    local attval1=role.GetRoleAttValue(MapID, InstanceID, RoleID, 1)
				local attval2=role.GetRoleAttValue(MapID, InstanceID, RoleID, 3)
				if attval1 > attval2 then
				    role.AddRoleItem(MapID, InstanceID, RoleID, 8600099, 1, 5, 13, 1001)  --法宝
				else
				    role.AddRoleItem(MapID, InstanceID, RoleID, 8600098, 1, 5, 13, 1001)  --法宝
				end
				act.SetActScriptData(151, 1, k+20, 1)
				act.SaveActScriptData(151)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12129) -- "金哥：\n        请您保证自己的行囊中有足够的空间再来领取。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		end
	elseif TalkIndex == 10 then
	    local k = nil
		for i = 0, 9 do
		    if act.GetActScriptData(151, 1, i) ~= nil and act.GetActScriptData(151, 1, i) > 0 and act.GetActScriptData(151, 1, i) == RoleID then
			    k = i
				break
			end
		end
		if k == nil then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12127) -- "金哥：\n        很抱歉，您没有进入排行榜。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif act.GetActScriptData(151, 1, k+20) == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12128) -- "金哥：\n        对不起，您已经领取过奖励了。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
			if role.GetBagFreeSize(RoleID) > 0 then
				role.AddRoleItem(MapID, InstanceID, RoleID, 3305007, 1, -1, 13, 1001)  --称号
				act.SetActScriptData(151, 1, k+20, 1)
				act.SaveActScriptData(151)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12129) -- "金哥：\n        请您保证自己的行囊中有足够的空间再来领取。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		end
    end
end

aux.RegisterCreatureEvent(4900490, 7, "n4900490_OnTalk")

function C4900492_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if -1 == TalkIndex then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12130) -- "下凡牛郎：\n        您可以在我这里用金银梭兑换姻缘结。\n        悄悄地跟你说一声，如果有情侣在我周围使用亲吻的交互动作的话，将会得到我的祝福，得到一个放牛绳。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- “兑换姻缘结”
		msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 22) -- “取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	elseif 4 == TalkIndex then
	    local i = role.GetRoleItemNum(RoleID, 3305002)
		if i < 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12131) -- "下凡牛郎：\n        兑换姻缘结需要一个金银梭，请您准备好后再来。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif role.GetBagFreeSize(RoleID) == 0 then
			    msg.AddMsgEvent(MsgID, 20, 12132) -- "下凡牛郎：\n        请您保证自己的行囊中有足够的空间再来兑换。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
		    role.RemoveFromRole(MapID, InstanceID, RoleID, 3305002, 1, 420)
            role.AddRoleItem(MapID, InstanceID, RoleID, 3305003, 1, -1, 8, 420)
			HuoDeYinYuanJie(RoleID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12133) -- "下凡牛郎：\n        您还要继续兑换吗？"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “兑换姻缘结”
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 22) -- “取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	end
end

--注册兑换奖励对话事件
aux.RegisterCreatureEvent(4900492, 7, "C4900492_OnTalk")

function C4900493_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if -1 == TalkIndex then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12134) -- "下凡织女：\n        您可以在我这里用放牛绳兑换姻缘结。\n        悄悄地跟你说一声，如果有情侣在我周围使用亲吻的交互动作的话，将会得到我的祝福，得到一个放牛绳。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- “兑换姻缘结”
		msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 22) -- “取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	elseif 4 == TalkIndex then
	    local i = role.GetRoleItemNum(RoleID, 3305001)
		if i < 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12135) -- "下凡织女：\n        兑换姻缘结需要一个放牛绳，请您准备好后再来。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		elseif role.GetBagFreeSize(RoleID) == 0 then
			    msg.AddMsgEvent(MsgID, 20, 12136) -- "下凡织女：\n        请您保证自己的行囊中有足够的空间再来兑换。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, -1) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- “确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
		    role.RemoveFromRole(MapID, InstanceID, RoleID, 3305001, 1, 420)
            role.AddRoleItem(MapID, InstanceID, RoleID, 3305003, 1, -1, 8, 420)
			HuoDeYinYuanJie(RoleID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 12137) -- "下凡织女：\n        您还要继续兑换吗？"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 21) -- “兑换姻缘结”
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 22) -- “取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	end
end

--注册兑换奖励对话事件
aux.RegisterCreatureEvent(4900493, 7, "C4900493_OnTalk")


--七夕香水礼包
function I3305005_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3303308, 3, -1, 8, 420)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5 + 1)
	if k > 23 then
	    k = 23
	end
	if k <= 23 then
        role.AddRoleExp(MapID, InstanceID, RoleID, SanBa_exp[k]*3)
	end
	local d = math.random(4)
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3303048+d, 1, -1, 8, 420)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 12138)   --您收到了来自于XXX的礼品——XXXX香水。
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 4, 3303048+d)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
end

function I3305005_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	local sex = role.GetRoleSex(TargetID)
	if(sex == 1) then

		return 31, bIgnore
	end
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		bRet = 31
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3305005, 1, "I3305005_OnUse")
aux.RegisterItemEvent(3305005, 0, "I3305005_CanUse")


--七夕时装礼包
function I3305006_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local Target_MapID, Target_InstanceID = role.GetRoleMapID(TargetID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5 + 1)
	if k > 23 then
	    k = 23
	end
	if k <= 23 then
        role.AddRoleExp(MapID, InstanceID, RoleID, SanBa_exp[k]*3)
	end
	role.AddRoleItem(Target_MapID, Target_InstanceID, TargetID, 3305004, 1, -1, 8, 420)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 12138)   --您收到了来自于XXX的礼品——XXXX香水。
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.AddMsgEvent(MsgID, 4, 3305004)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)     --发送消息
end

function I3305006_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	local sex = role.GetRoleSex(TargetID)
	if(sex == 0) then

		return 31, bIgnore
	end
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		bRet = 31
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3305006, 1, "I3305006_OnUse")
aux.RegisterItemEvent(3305006, 0, "I3305006_CanUse")

--时装随机包
function I3305004_OnUse(MapID, InstanceID, TypeID, RoleID)
    local k = math.random(36)
	role.AddRoleItem(MapID, InstanceID, RoleID, 9141048+k, 1, -1, 8, 420)
end

function I3305004_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)

	if(FreeSize < 1) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		bRet = 32
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(3305004, 1, "I3305004_OnUse")
aux.RegisterItemEvent(3305004, 0, "I3305004_CanUse")
