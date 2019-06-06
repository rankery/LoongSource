GM_bangpaihuodong_on = 0
GM_bangpaihuodong_t = 0
GM_bangpaihuodong_h = 0
GM_bangpaihuodong_m = 0

GM_bangpaihuodong_pos = {}
GM_bangpaihuodong_pos[1]={x=1160, y=12645, z=1765}
GM_bangpaihuodong_pos[2]={x=1237, y=12470, z=1788}
GM_bangpaihuodong_pos[3]={x=1248, y=12612, z=1809}
GM_bangpaihuodong_pos[4]={x=1270, y=12596, z=1802}
GM_bangpaihuodong_pos[5]={x=1276, y=12852, z=1831}
GM_bangpaihuodong_pos[6]={x=1250, y=12860, z=1834}
GM_bangpaihuodong_pos[7]={x=1265, y=12884, z=1854}
GM_bangpaihuodong_pos[8]={x=1286, y=12847, z=1865}
GM_bangpaihuodong_pos[9]={x=1250, y=12825, z=1887}
GM_bangpaihuodong_pos[10]={x=1270, y=12588, z=1921}
GM_bangpaihuodong_pos[11]={x=1236, y=12476, z=1975}
GM_bangpaihuodong_pos[12]={x=1213, y=12613, z=1980}
GM_bangpaihuodong_pos[13]={x=1208, y=12709, z=1933}
GM_bangpaihuodong_pos[14]={x=1190, y=12859, z=1899}
GM_bangpaihuodong_pos[15]={x=1168, y=13134, z=1886}
GM_bangpaihuodong_pos[16]={x=1187, y=12959, z=1872}
GM_bangpaihuodong_pos[17]={x=1194, y=12918, z=1858}
GM_bangpaihuodong_pos[18]={x=1179, y=13099, z=1835}
GM_bangpaihuodong_pos[19]={x=1186, y=12919, z=1822}
GM_bangpaihuodong_pos[20]={x=1172, y=12772, z=1798}
GM_bangpaihuodong_pos[21]={x=1144, y=13346, z=1810}
GM_bangpaihuodong_pos[22]={x=1157, y=13341, z=1844}
GM_bangpaihuodong_pos[23]={x=1147, y=13383, z=1874}
GM_bangpaihuodong_pos[24]={x=1134, y=13492, z=1868}
GM_bangpaihuodong_pos[25]={x=1127, y=13243, z=1897}
GM_bangpaihuodong_pos[26]={x=1134, y=12986, z=1915}
GM_bangpaihuodong_pos[27]={x=1129, y=12596, z=1946}
GM_bangpaihuodong_pos[28]={x=1142, y=12669, z=1937}
GM_bangpaihuodong_pos[29]={x=1088, y=12824, z=1934}
GM_bangpaihuodong_pos[30]={x=1071, y=13052, z=1917}
GM_bangpaihuodong_pos[31]={x=1060, y=12979, z=1937}
GM_bangpaihuodong_pos[32]={x=1035, y=13295, z=1916}
GM_bangpaihuodong_pos[33]={x=1111, y=13464, z=1802}
GM_bangpaihuodong_pos[34]={x=1125, y=13361, z=1780}

GM_bangpaihuodong_rank = {}

function GM_bangpaihuodong(MapID, InstanceID, TypeID, TargetID)

	GM_bangpaihuodong_rank[1] = {GuildID=0, num=0, rank=1, lastkiller=0}
	GM_bangpaihuodong_rank[2] = {GuildID=0, num=0, rank=2, lastkiller=0}
	GM_bangpaihuodong_rank[3] = {GuildID=0, num=0, rank=3, lastkiller=0}
	GM_bangpaihuodong_rank[4] = {GuildID=0, num=0, rank=4, lastkiller=0}
	GM_bangpaihuodong_rank[5] = {GuildID=0, num=0, rank=5, lastkiller=0}
	GM_bangpaihuodong_rank[6] = {GuildID=0, num=0, rank=6, lastkiller=0}
	GM_bangpaihuodong_rank[7] = {GuildID=0, num=0, rank=7, lastkiller=0}
	GM_bangpaihuodong_rank[8] = {GuildID=0, num=0, rank=8, lastkiller=0}
	GM_bangpaihuodong_rank[9] = {GuildID=0, num=0, rank=9, lastkiller=0}
	GM_bangpaihuodong_rank[10] = {GuildID=0, num=0, rank=10, lastkiller=0}
	GM_bangpaihuodong_rank[11] = {GuildID=0, num=0, rank=11, lastkiller=0}
	GM_bangpaihuodong_rank[12] = {GuildID=0, num=0, rank=12, lastkiller=0}
	GM_bangpaihuodong_rank[13] = {GuildID=0, num=0, rank=13, lastkiller=0}
	GM_bangpaihuodong_rank[14] = {GuildID=0, num=0, rank=14, lastkiller=0}
	GM_bangpaihuodong_rank[15] = {GuildID=0, num=0, rank=15, lastkiller=0}
	GM_bangpaihuodong_rank[16] = {GuildID=0, num=0, rank=16, lastkiller=0}
	GM_bangpaihuodong_rank[17] = {GuildID=0, num=0, rank=17, lastkiller=0}
	GM_bangpaihuodong_rank[18] = {GuildID=0, num=0, rank=18, lastkiller=0}
	GM_bangpaihuodong_rank[19] = {GuildID=0, num=0, rank=19, lastkiller=0}
	GM_bangpaihuodong_rank[20] = {GuildID=0, num=0, rank=20, lastkiller=0}

	GM_bangpaihuodong_on = 1
	GM_bangpaihuodong_t = 0
	GM_bangpaihuodong_h = tonumber(os.date("%H"))
	GM_bangpaihuodong_m = tonumber(os.date("%M"))

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1813)	--帮派活动之“激战！涿鹿神迹”已开启，在30分钟内在涿鹿神迹周围击杀神迹凶兽数量前五名的帮派
	--可获得2000点帮派资材奖励！
	msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong2_t))
	GM_fubenhuodong2_t=1
	msg.DispatchWorldMsgEvent(MsgID)

	for i=1, 34 do

		local monlevel = 0
		local temp = math.random(100)

		if temp<=20 then monlevel=1		--45级怪
		elseif temp<=40 then monlevel=2	--50级怪
		elseif temp<=55 then monlevel=3	--55级怪
		elseif temp<=70 then monlevel=4	--60级怪
		elseif temp<=72 then monlevel=5	--65级怪
		elseif temp<=84 then monlevel=6	--70级怪
		elseif temp<=92 then monlevel=7	--75级怪
		elseif temp<=100 then monlevel=8	--80级怪
		end

		map.MapCreateCreature(MapID, InstanceID, 1545000+monlevel, GM_bangpaihuodong_pos[i].x, GM_bangpaihuodong_pos[i].y, GM_bangpaihuodong_pos[i].z)
	end


end

aux.RegisterItemEvent(1131103, 1, "GM_bangpaihuodong")

--神迹凶兽死亡事件
function Shenjixiongshou_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)


	if GM_bangpaihuodong_on == 1 then

		local monlevel = 0
		local temp = math.random(100)
		local randompos = math.random(34)

		if temp<=20 then monlevel=1		--45级怪
		elseif temp<=40 then monlevel=2	--50级怪
		elseif temp<=55 then monlevel=3	--55级怪
		elseif temp<=70 then monlevel=4	--60级怪
		elseif temp<=72 then monlevel=5	--65级怪
		elseif temp<=84 then monlevel=6	--70级怪
		elseif temp<=92 then monlevel=7	--75级怪
		elseif temp<=100 then monlevel=8	--80级怪
		end

		map.MapCreateCreature(MapID, InstanceID, 1545000+monlevel, GM_bangpaihuodong_pos[randompos].x, GM_bangpaihuodong_pos[randompos].y, GM_bangpaihuodong_pos[randompos].z)

		local GuildID = guild.GetRoleGuildID(RoleID)
		if GuildID~=4294967295 then

			local guildindex = 0

			for i=1, 20 do
				if GM_bangpaihuodong_rank[i].GuildID==0 or GM_bangpaihuodong_rank[i].GuildID==GuildID then
					guildindex = i
					break
				end
			end

			if guildindex~= 0 then
				GM_bangpaihuodong_rank[guildindex].GuildID=GuildID
				GM_bangpaihuodong_rank[guildindex].lastkiller=RoleID
				GM_bangpaihuodong_rank[guildindex].num = GM_bangpaihuodong_rank[guildindex].num+1

				if GM_bangpaihuodong_rank[guildindex].rank~=1 then

					local compareindex=0

					for i=1,20 do
						if GM_bangpaihuodong_rank[i].rank == GM_bangpaihuodong_rank[guildindex].rank - 1 then
							compareindex=i
							break
						end
					end

					if GM_bangpaihuodong_rank[guildindex].num>GM_bangpaihuodong_rank[compareindex].num then
						GM_bangpaihuodong_rank[guildindex].rank=GM_bangpaihuodong_rank[guildindex].rank-1
						GM_bangpaihuodong_rank[compareindex].rank=GM_bangpaihuodong_rank[compareindex].rank+1
					end

				end

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1815)--您的帮派已击杀神迹凶兽X只，当前排名：X。
				msg.AddMsgEvent(MsgID, 9, GM_bangpaihuodong_rank[guildindex].num)
				msg.AddMsgEvent(MsgID, 9, GM_bangpaihuodong_rank[guildindex].rank)
				msg.DispatchGuildMsgEvent(MsgID, GuildID)

			else

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1816)--参与本次活动的帮派已满，您击杀神迹凶兽无法获得奖励！
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			end

		end
	end

end

aux.RegisterCreatureEvent(1545001, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545002, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545003, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545004, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545005, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545006, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545007, 4, "Shenjixiongshou_OnDie")
aux.RegisterCreatureEvent(1545008, 4, "Shenjixiongshou_OnDie")
