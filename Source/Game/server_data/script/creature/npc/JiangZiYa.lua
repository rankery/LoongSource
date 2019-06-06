--NPC钓鱼传送人脚本

function JiangZiYa_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if rolelevel < 20 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 282)	-- "龙隐村内，垂钓溪边，山野闲人，享尽天伦……"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if MapID== 2983744207 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 276)		--“哈哈，侠士看上去精神抖擞，是否已经体会到池边垂钓修身养性的妙处了呢？侠士若要回到玄天龙城，尽可知会老夫。”  在m20
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	279)		--“那我也来做一回渔翁！
				msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	281)		--“还是算了……”
				msg.AddMsgEvent(MsgID, 21,	6)			--查询选项
				msg.AddMsgEvent(MsgID, 1,	285)		--“查询钓鱼熟练度”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			elseif MapID== 3017298127 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 277)		--“侠士满脸疲倦，显是受尽了沙场征战的劳顿。何不学学老夫，偏居一隅，山水之间独享垂钓之乐。不但同征战沙场一样可获得经验修炼，也许还能找到什么奇珍异宝也未可知啊！”  在m02
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	278)		--“我要回到玄天龙城
				msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	281)		--“还是算了……”
				msg.AddMsgEvent(MsgID, 21,	6)			--查询选项
				msg.AddMsgEvent(MsgID, 1,	285)		--“查询钓鱼熟练度”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 4 then
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if MapID== 2983744207 and rolelevel>=20 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298127, 2214, 5738, 2132)
		elseif MapID== 3017298127 and rolelevel>=20 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983744207, 964, 10095, 2001)
		end
		TalkIndex = -1
	elseif TalkIndex == 6 then
		local a = role.GetRoleScriptData(RoleID, 1, RoleDataType["FishingLevel"])
		local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FishingNum"])
		if a == 0 then
			a = 1
		end
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 286) --您的钓鱼熟练度等级为<p1>,还差<p2>点数升级
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 9, a*50-b)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		TalkIndex = -1
	end
end

-- 注册
aux.RegisterCreatureEvent(3000023, 7, "JiangZiYa_OnTalk")

