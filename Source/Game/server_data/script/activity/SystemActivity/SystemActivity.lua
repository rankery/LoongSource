--系统计时活动
--活动Scriptdata 1 2 3 4 5 6 7 8 9 10 11 12已经被飞升使用
--活动Scriptdata 13 14 15已经被竞技场使用
--活动Scriptdata 16已经被帮主活动使用
--活动Scriptdata 17已经被龙之穴活动使用
--活动Scriptdata 40 41已经被竞技场超神器购买占用
--活动scriptdata 42 用于清空七夕和开学活动的数据

guangbo_baizhan = 0  --记录在1分钟内是否有人触发了全服广播
shixian_hecheng = 0  --记录合成商人的id
shenyunfei_ID = 0
dragon_timer = 0
dragon_timerbegin = 0

systemactivity_timermin =0

function SystemActivity_OnTimerMin(actID)
    local curmon = tonumber(os.date("%m"))
	local curday = tonumber(os.date("%d"))
	local curmin = tonumber(os.date("%M"))
	local curhour = tonumber(os.date("%H"))
	local curweekday = tonumber(os.date("%w"))


	systemactivity_timermin= systemactivity_timermin+1

	--服务器维护补偿-龙之穴

	if systemactivity_timermin==20 and LongZhiXue_Buchang == 1 then

		local bossindex = act.GetActScriptData(99, 1, 17)

		if bossindex==1 then
			map.MapCreateCreature(3017298383, -1, 1550201, 2054, 10964, 2544)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3630)		--补偿神龙已在凤翔出现
			msg.DispatchWorldMsgEvent(MsgID)

		elseif bossindex==2 then
			map.MapCreateCreature(3017299663, -1, 1550202, 1166, 13219, 1873)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3631)		--补偿神龙已在凤翔出现
			msg.DispatchWorldMsgEvent(MsgID)

		elseif bossindex==3 then
			map.MapCreateCreature(3017299919, -1, 1550203, 1803, 24427, 2568)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3632)		--补偿神龙已在凤翔出现
			msg.DispatchWorldMsgEvent(MsgID)

		elseif bossindex==4 then
			map.MapCreateCreature(3017299151, -1, 1550204, 1915, 3100, 761)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3633)		--补偿神龙已在凤翔出现
			msg.DispatchWorldMsgEvent(MsgID)

		end

	end

	--GM裕祥山麓活动

	if GM_fubenhuodong_on == 1 then

		if curmin == GM_fubenhuodong_m and curhour == (GM_fubenhuodong_h+3)%24 then

			GM_fubenhuodong_on = 0

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1810)	--新服活动之“镇压！裕祥山麓”已结束！
			msg.DispatchWorldMsgEvent(MsgID)

		else
			if curmin == (GM_fubenhuodong_m+30)%60 then

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1809)	--新服活动之“镇压！裕祥山麓”已开启，在x分钟内所有挑战裕祥山麓副本并击杀首领“风刃霸山”
				--的玩家均可获得最高100000点的经验奖励！
				msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong_t))
				GM_fubenhuodong_t=GM_fubenhuodong_t+1
				msg.DispatchWorldMsgEvent(MsgID)
			elseif curmin == GM_fubenhuodong_m and curhour ~= GM_fubenhuodong_h then

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1809)	--新服活动之“镇压！裕祥山麓”已开启，在x分钟内所有挑战裕祥山麓副本并击杀首领“风刃霸山”
				--的玩家均可获得最高100000点的经验奖励！
				msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong_t))
				GM_fubenhuodong_t=GM_fubenhuodong_t+1
				msg.DispatchWorldMsgEvent(MsgID)
			end
		end
	end

	--GM枭吴祭坛活动

	if GM_fubenhuodong2_on == 1 then

		if curmin == GM_fubenhuodong2_m and curhour == (GM_fubenhuodong2_h+3)%24 then

			GM_fubenhuodong2_on = 0

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1812)	--新服活动之“镇压！枭吴祭坛”已结束！
			msg.DispatchWorldMsgEvent(MsgID)

		else
			if curmin == (GM_fubenhuodong2_m+30)%60 then

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1811)	--新服活动之“镇压！裕祥山麓”已开启，在x分钟内所有挑战裕祥山麓副本并击杀首领“风刃霸山”
				--的玩家均可获得最高100000点的经验奖励！
				msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong2_t))
				GM_fubenhuodong_t=GM_fubenhuodong2_t+1
				msg.DispatchWorldMsgEvent(MsgID)
			elseif curmin == GM_fubenhuodong2_m and curhour ~= GM_fubenhuodong2_h then

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1811)	--新服活动之“镇压！裕祥山麓”已开启，在x分钟内所有挑战裕祥山麓副本并击杀首领“风刃霸山”
				--的玩家均可获得最高100000点的经验奖励！
				msg.AddMsgEvent(MsgID, 9, 30*(6-GM_fubenhuodong2_t))
				GM_fubenhuodong_t=GM_fubenhuodong2_t+1
				msg.DispatchWorldMsgEvent(MsgID)
			end
		end
	end

	--GM涿鹿神迹活动

	if GM_bangpaihuodong_on == 1 then

		local temp_flag = 0

		if curmin == GM_bangpaihuodong_m and curhour == GM_bangpaihuodong_h then
			temp_flag=1
		end

		if temp_flag==0 and curmin==(GM_bangpaihuodong_m+30)%60 then

			GM_bangpaihuodong_on=0

			for i=1,20 do
				if GM_bangpaihuodong_rank[i].rank<=5 then
					guild.ModifyGuildMaterial(GM_bangpaihuodong_rank[i].GuildID, GM_bangpaihuodong_rank[i].lastkiller, 2000, 102)
				end
			end

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1814)	--帮派活动之“激战！涿鹿神迹”已结束，排名前五的帮派已获得2000帮派资材奖励！
			msg.DispatchWorldMsgEvent(MsgID)

		end
	end

	--竞技场相关
	if Open_Arena == 1 and curmin == 30 and curhour % 2 == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 2537)    --以武会友，勇夺神器——竞技场系统已经开放，大家可以到涿鹿的【2071，2666】跟竞技场管事对话进入竞技场。
		msg.DispatchWorldMsgEvent(MsgID)
	end

	--英雄乱舞
	if PVPactivity_trigger == 1 then
		if day == "Monday" or day == "Friday" or day == "Wednesday" or day == "Sunday" then
			if curhour == 7 and curmin >= 30 and curmin % 10 == 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100107)    --英雄乱武活动即将于<color=0xffff0000>晚8点<color=0xfffff7e0>准时开始！
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			end
		--[[elseif day == "Wednesday" or day == "Sunday" then
			if curhour == 7 and curmin >= 50 and curmin <= 59 then
			    if IsCreateAct18Instance == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2861)   --活动尚未开始，您无法执行该操作
					msg.DispatchMapMsgEvent(MsgID, 2983743695, 1, -1)
				end
				if IsCreateAct19Instance == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2861)   --活动尚未开始，您无法执行该操作
					msg.DispatchMapMsgEvent(MsgID, 2983743695, 2, -1)
				end
				if IsCreateAct20Instance == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2861)   --活动尚未开始，您无法执行该操作
					msg.DispatchMapMsgEvent(MsgID, 2983743695, 3, -1)
				end
				if IsCreateAct21Instance == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 2861)   --活动尚未开始，您无法执行该操作
					msg.DispatchMapMsgEvent(MsgID, 2983743695, 4, -1)
				end
			end
			if curhour == 7 and curmin >= 30 and curmin % 10 == 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100110)    --群雄逐鹿活动即将于<color=0xffff0000>晚8点<color=0xfffff7e0>准时开始，请大家在7点50之前前往报名！
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			end]]
		end
    end

	--城战加冕人

	if curweekday==6 and curhour==22 and curmin==30 then
		map.MapCreateColCreature(3695619387, -1, 4500705, 800, 10350, 590)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3028)    --凤翔城主加冕人已现身凤翔禁城————凤翔承天府，请新任城主速速前往受封！
		msg.DispatchWorldMsgEvent(MsgID)
	end

	--百斩修罗和众志成城
    guangbo_baizhan = 0

	--副本经验领取广播，每两分钟最多有一次
	if curmin % 2 == 0 then
        JingYanLingQu_FB = 0
	end

	--[[最早飞升玩家广播
	if curmin == 20 or curmin == 50 then
		if FeiSheng_trigger == 1 and act.GetActScriptData(99, 1, 5) ~= 0 then
			for i= 7,11 do
				if act.GetActScriptData(99, 1, i) ~= nil and act.GetActScriptData(99, 1, i) > 0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 102, 100122)
					msg.AddMsgEvent(MsgID, 2, act.GetActScriptData(99, 1, i-6))
					msg.AddMsgEvent(MsgID, 9, i-6)
					msg.DispatchBroadcast(MsgID,-1,-1,-1)
					act.SetActScriptData(99, 1, i, act.GetActScriptData(99, 1, i)-1)
				    act.SaveActScriptData(99)
				end
			end
		end
	end]]

    --每周五晚上20点15刷出实现制装备合成商人
	if (curweekday==5 and ((curhour==20 and curmin>=15) or curhour>=21)) or (curweekday==6 and ((curhour==20 and curmin<=15) or curhour<=19)) then
	    if shixian_hecheng == 0 then--记录合成商人的id
			shixian_hecheng = map.MapCreateColCreature(3017298383, -1, 4900486, 991, 10000, 1985)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2551)    --时限制装备合成商人已经出现在凤翔的【991,1985】，请在24小时内前往合成！
			msg.DispatchWorldMsgEvent(MsgID)
		end
	else
	    if shixian_hecheng ~= 0 then
		    map.MapDeleteCreature(3017298383, -1, shixian_hecheng)
			shixian_hecheng = 0
		end
	end

	--每两个小时刷出12个火麒麟
	if curhour % 2 == 0 and curmin == 0 then
		if FeiSheng_trigger == 1 then          --and act.GetActScriptData(99, 1, 12) == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3178)    --火麒麟已经出现在昆仑，击杀它们可以获得飞升凭证——赤金火纹鬃！
			msg.DispatchMapMsgEvent(MsgID, 3017299407, -1)
			--map.MapCreateColCreature(3017299407, -1, 1535326, feisheng_qilin[1].x, feisheng_qilin[1].y, feisheng_qilin[1].z, 1, "")
			for i= 2,11 do
				map.MapCreateColCreature(3017299407, -1, 1535327, feisheng_qilin[i].x, feisheng_qilin[i].y, feisheng_qilin[i].z, 1, "")
			end
		end
	end
	if (curhour == 11 or curhour == 20 or curhour == 1) and curmin == 30 then
		if FeiSheng_trigger == 1 then -- and act.GetActScriptData(99, 1, 12) == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100126)    --赤火麒麟已经出现在昆仑，击杀它们可以获得珍贵的强化道具和飞升凭证——赤金火纹鬃！
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
			--map.MapCreateColCreature(3017299407, -1, 1535326, feisheng_qilin[1].x, feisheng_qilin[1].y, feisheng_qilin[1].z, 1, "")
    	end
	end
	if (curhour == 12 or curhour == 21 or curhour == 2) and curmin == 0 then
		if FeiSheng_trigger == 1 then    --and act.GetActScriptData(99, 1, 12) == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100123)    --赤火麒麟已经出现在昆仑，击杀它们可以获得珍贵的强化道具和飞升凭证——赤金火纹鬃！
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
			map.MapCreateColCreature(3017299407, -1, 1535326, feisheng_qilin[1].x, feisheng_qilin[1].y, feisheng_qilin[1].z, 1, "")
    	end
	end

	--有人飞升后每周日下午葵灵宝树活动
	if FeiSheng_trigger == 1 and act.GetActScriptData(99, 1, 1) ~= 0 then
	    if curweekday == 0 and curhour % 2 == 0 and curhour >= 6  and curhour <= 14 and curmin == 5 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3132)    --今天下午一点整，白眉仙官将会在凌霄天宫之下栽种葵灵宝树，届时所有40级以上的玩家都可以前去相助！
			msg.DispatchWorldMsgEvent(MsgID)
		end
		if curweekday == 0 and ((curhour==13 or (curhour==14 and curmin <= 30)) or KuiLingBaoShu_Start == 1) then
		    KuiLingBaoShu_min = KuiLingBaoShu_min + 1
		    KuiLingBaoShu(curweekday, curhour, curmin, KuiLingBaoShu_min)
		end
	end


	--每天的中午12:00,12:05,12:10,3:00，6:00广播可以领取令牌开展帮派活动。
	if (curhour == 12 and (curmin == 0 or curmin == 5 or curmin == 10)) or ((curhour == 15 or curhour == 18) and curmin == 0) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100127)    --帮派的帮主每天12:00之后可以在凤翔城战场景中的城务发布人处领取一个“清剿匪类令”，使用该令牌将可以为自己帮派的成员开辟一个清剿匪类的帮派练功副本。
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end

	if city.GetCityAppointedAtt(3, 2) ~= nil and city.GetCityAppointedAtt(3, 2) ~= 4294967295 then
	    --每天早上0:00将可以领取城主奖励的时间减一。

        if curhour == 0 and curmin == 0 then
			if act.GetActScriptData(99, 1, 16) > 0 then
				act.SetActScriptData(99, 1, 16, act.GetActScriptData(99, 1, 16)-1)
   			end
		elseif (curhour == 18 or curhour == 19 or curhour == 20) and curmin == 5 then
		    if act.GetActScriptData(99, 1, 16) > 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100129)    --凤翔城主决定广施恩惠，之后三天晚上18:00-21:00所有玩家可以前往城战场景在城务发布人处领取经验奖励！
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			end
        end

		--时间到刷出裂地魔跟班
		if curhour == 16 and curmin == 0 and curweekday == 6 then
		    map.MapCreateColCreature(3695619387, -1, 4900709, chengzhu_pos[1].x, chengzhu_pos[1].y, chengzhu_pos[1].z, 1, "")
	        local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100130)    --裂地魔向凤翔城主送来了战书！所有玩家都可以赶到凤翔战场，如果凤翔城主在XX点之前取得胜利，则所有观战的玩家都可以从裂地魔跟班处获得高额的经验奖励！
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
		elseif curhour == 16 and curmin % 10 == 0 and Liedimo_dead == 0 and curweekday == 6 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100131)    --裂地魔向凤翔城主送来了战书！所有玩家都可以赶到凤翔战场，如果凤翔城主在XX点之前取得胜利，则所有观战的玩家都可以从裂地魔跟班处获得高额的经验奖励！
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
		end
	end

	if (curmon == 7 and curday > 10) or (curmon == 8 and curday <= 2) then
		if ((curhour == 20 and curmin >= 30) or (curhour == 21 and curmin < 30)) and shenyunfei_ID == 0 then
		    shenyunfei_ID = map.MapCreateColCreature(3017298383, -1, 3610219, 923, 10000, 2007, 1, "")
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100132)    --七月份中下旬的晚上20:30-21:30《龙》小说的男主角沈云飞将会出现在凤翔的【923,2007】，完成他给予的任务可以获得附加行囊或者随身仓库！（该任务每天可以完成一次，每人累计最多可以完成3次）
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
		end

		if (curhour == 20 and curmin == 45) or (curhour == 21 and curmin == 15) then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100132)    --七月份中下旬的晚上20:30-21:30《龙》小说的男主角沈云飞将会出现在凤翔的【923,2007】，完成他给予的任务可以获得附加行囊或者随身仓库！（该任务每天可以完成一次，每人累计最多可以完成3次）
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
		elseif curhour == 21 and curmin == 30 then
		    map.MapDeleteCreature(3017298383, -1, shenyunfei_ID)
			shenyunfei_ID = 0
		end
	end

	-- 中秋节期间，每日的20:00清空砍伐树木的数量

	if Zhongqiujie == 1	then

		if (curmon == 9 and curday == 22)	and  (curhour== 19 and curmin==55)  then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3901)	--中秋活动再过5分钟就要开始了,请各位玩家到巫水找到：嫦娥[1000,969]、吴刚[955，970]、桂树[976，969]与他们共度佳节；
		msg.DispatchWorldMsgEvent(MsgID)
		end

		if 	curhour == 20 and curmin == 0 then
			zhongqiu_kanfahenji = 0
		end
		if 	curhour >=19 and curhour <=21 then
			yueguangbaohe  = 1
		else
			yueguangbaohe  = 0
		end

	end
	if Guoqingjie == 1 then
		if (curmon == 9 and curday == 28) and (curhour == 17 and curmin == 55) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4001)	--再过5分钟，国庆活动就要开始了，祝大家国庆节愉快，并欢迎大家踊跃参与；
			msg.DispatchWorldMsgEvent(MsgID)
		end
		if curhour == 10 and  curmin == 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4027)	--国庆怪物阅兵开始了，请大家到 巫水【1677,701】参观怪物阅兵
			msg.DispatchWorldMsgEvent(MsgID)
		end
		if curhour == 19 and curmin == 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4028)	--国庆龙神阅兵开始了，请大家到 涿鹿【2106,2677】参观阅兵
			msg.DispatchWorldMsgEvent(MsgID)
		end

	end

	--[[劳动节装备萃取炉，材料凝炼炉 刷新时间
	if act.GetActIsStart(134) ~= 1 and (curweekday == 2 or curweekday == 4 or curweekday == 6 or curweekday == 0)then
		if curhour >= 18 and curhour <= 22  and Labor_Creature[1].CreID == 0 then
			for k = 1,2 do
				Labor_Creature[k].CreID = map.MapCreateColCreature(3017299663, -1, Labor_Creature[k].TypeID, Labor_Creature[k].x, Labor_Creature[k].y, Labor_Creature[k].z, 1)
			end
		elseif Labor_Creature[1].CreID ~= 0 and curhour == 23 then
			for k = 1,2 do
				map.MapDeleteCreature(3017299663, -1, Labor_Creature[k].CreID)
				Labor_Creature[k].CreID = 0
			end
		end
	end]]
end

--注册该活动的相关事件
aux.RegisterActEvent(99, 4, "SystemActivity_OnTimerMin")

-- 城战定时更新
function CityWar_OnTimerSec(Sec, actID)

	if CityStruggle_IsStart == 0 then
		return;
	end

	-- 每隔3秒给招魂幡周围玩家添加buffer2029001
	-- 进攻防御联盟阵营玩家
	if Sec % 25 == 0 then
		if Role_Alliance ~= nil then
			for k, v in pairs(Role_Alliance) do
				--  0--进攻联盟
				if v == 0 then
					if Atk_ZhaoHunFan.use == 1 then
						-- 根据玩家和招魂幡坐标，计算距离，如果不超过750，给玩家加buffer
						local bAddBuffer = aux.CalUnitDist(city_id[actID].MapID, -1, k, Atk_ZhaoHunFan.CreatureID)
						if bAddBuffer ~= nil and bAddBuffer == 1 then
							unit.AddBuff(city_id[actID].MapID, -1, k, 2029001, k)
						end
					end
				-- 1--防御联盟
				elseif v == 1 then
					if Def_ZhaoHunFan.use == 1 then
						-- 根据玩家和招魂幡坐标，计算距离，如果不超过750，给玩家加buffer
						local bAddBuffer = aux.CalUnitDist(city_id[actID].MapID, -1, k, Def_ZhaoHunFan.CreatureID)
						if bAddBuffer ~= nil and bAddBuffer == 1 then
							unit.AddBuff(city_id[actID].MapID, -1, k, 2029001, k)
						end
					end
				end
			end
		end
	end
end

function Dragon_UseSkill()
	if dragon_timerbegin == 1 then
	    if dragon_timer % 5 == 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 3803)    --剧毒蘑菇已经被召唤出来，X秒后蘑菇会对附近的单位造成巨大伤害。
			msg.AddMsgEvent(MsgID, 9, dragon_timer/5)
			msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)
		end
		dragon_timer = dragon_timer - 1

		if dragon_timer < 1 then
		    dragon_timerbegin = 0
		end
	end
end


function SystemActivity_OnTimerSec(ActiveID, Sec)
       Dragon_UseSkill()
	-- 城战定时更新
	CityWar_OnTimerSec(Sec, 70) -- 凤翔禁城


--[[    --每5秒对于英雄乱舞和群雄逐鹿的地图中的玩家进行一次开红设置
    if act.GetActIsStart(14) or act.GetActIsStart(97) then
	    local Sec   = tonumber(os.date("%S"))
        if Sec % 60 == 0 then
		    local RoleTbl1 = map.GetMapAllRoleID(2983744463, 1)
			local RoleTbl2 = map.GetMapAllRoleID(2983744463, 2)
			local RoleTbl3 = map.GetMapAllRoleID(2983744463, 3)
			local RoleTbl4 = map.GetMapAllRoleID(2983744463, 4)
			if RoleTbl1 then
				for k,v in pairs(RoleTbl1) do
					--设置玩家为PK状态(用于客户端敌我判断）
                    if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
			if RoleTbl2 then
				for k,v in pairs(RoleTbl2) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
			if RoleTbl3 then
				for k,v in pairs(RoleTbl3) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
			if RoleTbl4 then
				for k,v in pairs(RoleTbl4) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, false)
					end
				end
			end
		end
    elseif act.GetActIsStart(18) or act.GetActIsStart(103) then
	    local Sec   = tonumber(os.date("%S"))
        if Sec % 60 == 0 then
		    local RoleTbl1 = map.GetMapAllRoleID(2983743695, 1)
			local RoleTbl2 = map.GetMapAllRoleID(2983743695, 2)
			local RoleTbl3 = map.GetMapAllRoleID(2983743695, 3)
			local RoleTbl4 = map.GetMapAllRoleID(2983743695, 4)
			if RoleTbl1 then
				for k,v in pairs(RoleTbl1) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
			if RoleTbl2 then
				for k,v in pairs(RoleTbl2) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
		    end
			if RoleTbl3 then
				for k,v in pairs(RoleTbl3) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
			if RoleTbl4 then
				for k,v in pairs(RoleTbl4) do
					--设置玩家为PK状态(用于客户端敌我判断）
					if role.IsRoleInStatus(v,4) ~= nil and role.IsRoleInStatus(v,4) ~= true then
					    role.SetRoleState(v, 4, true)
					end
				end
			end
		end
	end]]
end
--注册该活动的相关事件
aux.RegisterActEvent(99, 1, "SystemActivity_OnTimerSec")

function KuiLingBaoShu_OnClick(MapID, InstanceID, RoleID, TOMsgID, ToMapID, x, y, z)
    if TOMsgID == 3176 then
		if role.IsRoleInStatus(RoleID, 4096) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3177) -- 您正处于牢狱之中，无法使用该传送功能
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
			if KuiLingBaoShu_min == 1 or KuiLingBaoShu_min == 2 then
				if role.IsRoleInStatus(RoleID, 1024) then
					role.CancelRoleStallState(MapID, InstanceID, RoleID)
				end
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, ToMapID, x, y, z)      --传送
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 2779) -- 您点击确定的时间过晚，已经超出了1分钟的时间限制，请自行前往参加活动。
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(99, 8, "KuiLingBaoShu_OnClick")

