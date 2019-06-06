
function BaiYaoTa_CreatCreature(MapID, InstanceID, Wave, part)--刷出一波新怪
    local buff_trigger = math.random(100)
    if buff_trigger > 95 then
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for u,v in pairs(RoleTbl) do
			if u ~= nil then
				unit.AddBuff(MapID, InstanceID, v, 1130901, v)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 12351)    --百妖塔力量外泄，所有妖物和玩家的实力增强！
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
			end
		end
	end
	local MonPoint = {}
	MonPoint[1] = {x=125,y=40,z=341}
	MonPoint[2] = {x=124,y=40,z=355}
	MonPoint[3] = {x=130,y=40,z=327}
	MonPoint[4] = {x=112,y=40,z=350}
	MonPoint[5] = {x=109,y=40,z=334}
	MonPoint[6] = {x=118,y=40,z=324}
	MonPoint[7] = {x=140,y=40,z=331}
	MonPoint[8] = {x=137,y=40,z=355}
	MonPoint[9] = {x=124,y=40,z=367}
	MonPoint[10] = {x=104,y=40,z=361}
	MonPoint[11] = {x=100,y=40,z=345}
	MonPoint[12] = {x=102,y=40,z=320}
	MonPoint[13] = {x=122,y=40,z=311}
	MonPoint[14] = {x=136,y=40,z=318}
	MonPoint[15] = {x=150,y=40,z=341}
	MonPoint[16] = {x=139,y=40,z=370}
	MonPoint[17] = {x=113,y=40,z=377}
	MonPoint[18] = {x=95,y=40,z=361}
	MonPoint[19] = {x=89,y=40,z=331}
	MonPoint[20] = {x=108,y=40,z=304}
	MonPoint[21] = {x=134,y=40,z=305}
	MonPoint[22] = {x=152,y=40,z=321}
	MonPoint[23] = {x=151,y=40,z=358}
	MonPoint[24] = {x=130,y=40,z=383}
	MonPoint[25] = {x=105,y=40,z=375}
	MonPoint[26] = {x=87,y=40,z=348}
	MonPoint[27] = {x=91,y=40,z=318}
	MonPoint[28] = {x=122,y=40,z=294}
	MonPoint[29] = {x=151,y=40,z=305}
	MonPoint[30] = {x=159,y=40,z=348}
	MonPoint[31] = {x=149,y=40,z=377}
	MonPoint[32] = {x=117,y=40,z=388}
	MonPoint[33] = {x=94,y=40,z=375}
	MonPoint[34] = {x=85,y=40,z=361}
	MonPoint[35] = {x=79,y=40,z=334}
	MonPoint[36] = {x=97,y=40,z=300}
	MonPoint[37] = {x=135,y=40,z=292}
	MonPoint[38] = {x=161,y=40,z=328}
	MonPoint[39] = {x=162,y=40,z=367}
	MonPoint[40] = {x=136,y=40,z=391}
	MonPoint[41] = {x=105,y=40,z=391}
	MonPoint[42] = {x=84,y=40,z=373}
	MonPoint[43] = {x=75,y=40,z=351}
	MonPoint[44] = {x=83,y=40,z=314}
	MonPoint[45] = {x=108,y=40,z=290}
	MonPoint[46] = {x=128,y=40,z=284}
	MonPoint[47] = {x=149,y=40,z=293}
	MonPoint[48] = {x=164,y=40,z=311}
	MonPoint[49] = {x=170,y=40,z=344}
	MonPoint[50] = {x=155,y=40,z=387}

    local mon_wave = {}
	mon_wave[11] = {mon={1535416},num={20}}
	mon_wave[12] = {mon={1535417,1535416},num={2,20}}
	mon_wave[13] = {mon={1535417},num={6}}
	mon_wave[14] = {mon={1535418},num={1}}
	mon_wave[21] = {mon={1535419},num={25}}
	mon_wave[22] = {mon={1535420,1535419},num={3,25}}
	mon_wave[23] = {mon={1535420},num={8}}
	mon_wave[24] = {mon={1535421},num={1}}
	mon_wave[31] = {mon={1535422},num={30}}
	mon_wave[32] = {mon={1535423,1535422},num={4,30}}
	mon_wave[33] = {mon={1535423},num={10}}
	mon_wave[34] = {mon={1535424,1535425,1535426},num={1,1,1}}
	mon_wave[41] = {mon={1535427},num={35}}
	mon_wave[42] = {mon={1535428,1535427},num={5,35}}
	mon_wave[43] = {mon={1535428},num={14}}
	mon_wave[44] = {mon={1535429,1535430},num={1,1}}
	mon_wave[51] = {mon={1535431},num={40}}
	mon_wave[52] = {mon={1535432,1535431},num={6,40}}
	mon_wave[53] = {mon={1535432},num={20}}
	mon_wave[54] = {mon={1535433},num={1}}
	mon_wave[61] = {mon={1535446},num={50}}
	mon_wave[62] = {mon={1535447,1535446},num={10,40}}
	mon_wave[63] = {mon={1535447},num={25}}
	mon_wave[64] = {mon={1535448},num={1}}
	mon_wave[71] = {mon={1535449},num={50}}
	mon_wave[72] = {mon={1535450,1535449},num={10,40}}
	mon_wave[73] = {mon={1535450},num={30}}
	mon_wave[74] = {mon={1535451},num={1}}
	mon_wave[81] = {mon={1535452},num={50}}
	mon_wave[82] = {mon={1535453,1535452},num={10,40}}
	mon_wave[83] = {mon={1535453},num={35}}
	mon_wave[84] = {mon={1535454},num={1}}

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12352)	--    新的怪物已经出现
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	local k = Wave*10 + part

	local totle = 0
	for u,v in pairs(mon_wave[k].mon) do
	    for i= totle+1,mon_wave[k].num[u]+totle do
		    local monid = map.MapCreateCreature(MapID, InstanceID, v, MonPoint[i].x, MonPoint[i].y, MonPoint[i].z)
		    if buff_trigger > 95 then
			    unit.AddBuff(MapID, InstanceID, monid, 1130901, monid)
			end
		end
	    totle = mon_wave[k].num[u]+totle
	end

	map.SetMapScriptData(MapID, InstanceID, 1, 16, totle)
    map.SetMapScriptData(MapID, InstanceID, 1, 17, totle)
end
