
--地藏王使者对话脚本
function n3021003_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
	    local HaveQuest = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20161)
		if HaveQuest == true then
		    -- "地藏王使者：\\n    您已经接取了任务，请完成后再来。"
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 717)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
	        -- "地藏王使者：\\n    安忍不动如大地，静虑深密如秘藏。\\n    我主地藏菩萨曾立下宏愿：众生度尽，方证菩提。地狱不空，誓不成佛。少侠若是身上拥有<I>度魂符</I>，可在我处接取超度亡魂任务，助我主地藏菩萨渡化地狱亡魂，以抵消罪业，圆满功德。"
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 701)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		    msg.AddMsgEvent(MsgID, 21, 4) -- 任务选项
		    msg.AddMsgEvent(MsgID, 1, 702) -- "超度亡魂"
		    msg.AddMsgEvent(MsgID, 21, 5)   --任务选项
		    msg.AddMsgEvent(MsgID, 1, 703)   --“超度亡魂[同时捐献月光石精华]”
		    msg.AddMsgEvent(MsgID, 21, 6)   --任务选项
		    msg.AddMsgEvent(MsgID, 1, 704)   --“超度亡魂[同时捐献通冥祭魔石]”
		    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--点击接取普通任务
	elseif TalkIndex == 4 then
	    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if level < 55 then
	        --提示：“您的等级不足55级，任务接取失败。”
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 705)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	    else
	        --地藏王使者：\\n    若要超度亡魂还需为善去恶、积攒功德才好。如今天下大乱、盗匪妖魔横行，不知少侠是否愿意前往诛杀讨伐，以抵消罪业，普度众生？
	        local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 707)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		    msg.AddMsgEvent(MsgID, 21, 7)   --确定按钮
		    msg.AddMsgEvent(MsgID, 1, 708)   --“接取任务”
		    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--点击接取月光石任务
	elseif TalkIndex == 5 then
	    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if level < 55 then
	        --提示：“您的等级不足55级，任务接取失败。”
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 705)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	    else
	        --地藏王使者：\\n    若要超度亡魂还需为善去恶、积攒功德才好。如今巫水一地盗匪妖魔横行，若能予以诛杀讨伐，则必可抵消罪业，普度众生，又有月光石精华作衬，定能功德圆满，往生极乐！不知少侠是否愿意？
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 709)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		    msg.AddMsgEvent(MsgID, 21, 8)   --确定按钮
		    msg.AddMsgEvent(MsgID, 1, 710)   --“接取任务[同时捐献月光石精华]”
		    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--点击接取通冥祭魔石任务，显示对话
	elseif TalkIndex == 6 then
	    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if level < 55 then
	        --提示：“您的等级不足55级，任务接取失败。”
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 705)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	    else
            --地藏王使者：\\n    若要超度亡魂还需为善去恶、积攒功德才好。如今巫水一地盗匪妖魔横行，若能予以诛杀讨伐，则必可抵消罪业，普度众生，又有通冥祭魔石作衬，定能功德圆满，羽化登仙！不知少侠是否愿意？
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20, 711)
		    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		    msg.AddMsgEvent(MsgID, 21, 9)   --确定按钮
		    msg.AddMsgEvent(MsgID, 1, 712)   --“接取任务[同时捐献通冥祭魔石]”
		    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--接取普通任务
	elseif TalkIndex == 7 then
	    local DHF = role.GetRoleItemNum(RoleID, 2615902)
		if DHF > 0 then
		    if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 2615902, 1, 1000) then
			    role.SetRoleScriptData(RoleID, 1, RoleDataType["FDGJ_QuestType"], 1)   --设置普通经验类型
				role.AddQuest(RoleID, 20161)
				--您接取了“超度亡魂”任务
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 713)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    --提示：“您没有度魂符，无法接取该任务。”
		        local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20, 714)
		        msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		        msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		else
		    --提示：“您没有度魂符，无法接取该任务。”
		        local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20, 714)
		        msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		        msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--接取中等经验任务
    elseif TalkIndex == 8 then
	    local DHF = role.GetRoleItemNum(RoleID, 2615902)
		local YGSJH = role.GetRoleItemNum(RoleID, 3200704)
		if YGSJH > 0 then
		    if DHF > 0 then
		        if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 3200704, 1, 1000) then
		            if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 2615902, 1, 1000) then
					    role.SetRoleScriptData(RoleID, 1, RoleDataType["FDGJ_QuestType"], 2)  --设置中等经验类型
				        role.AddQuest(RoleID, 20161)
						--您接取了“超度亡魂”任务
						local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 26, 713)
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)
			        else
						--提示：“您没有度魂符，无法接取该任务。”
		                local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 20, 714)
		                msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			            msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		                msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			        end
				else
				    --提示：“您缺少月光石精华，无法选择此项。”
		                local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 20, 715)
		                msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			            msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		                msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
		    else
		        --提示：“您没有度魂符，无法接取该任务。”
			    local MsgID = msg.BeginMsgEvent()
			    msg.AddMsgEvent(MsgID, 20, 714)
			    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
			    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
			    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		    end
		else
			--提示：“您缺少月光石精华，无法选择此项。”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 715)
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--接取高等经验任务
    elseif TalkIndex == 9 then
	    local DHF = role.GetRoleItemNum(RoleID, 2615902)
		local TMJMS = role.GetRoleItemNum(RoleID, 3200602)
		if TMJMS > 0 then
		    if DHF > 0 then
		        if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 3200602, 1, 1000) then
		            if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 2615902, 1, 1000) then
					    role.SetRoleScriptData(RoleID, 1, RoleDataType["FDGJ_QuestType"], 3)  --设置高等经验类型
				        role.AddQuest(RoleID, 20161)
						--您接取了“超度亡魂”任务
						local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 26, 713)
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)
			        else
						--提示：“您没有度魂符，无法接取该任务。”
		                local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 20, 714)
		                msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			            msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		                msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			        end
				else
				    --提示：“您缺少通冥祭魔石，无法选择此项。”
		                local MsgID = msg.BeginMsgEvent()
		                msg.AddMsgEvent(MsgID, 20, 716)
		                msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			            msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
		                msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
		                msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
		    else
		        --提示：“您没有度魂符，无法接取该任务。”
			    local MsgID = msg.BeginMsgEvent()
			    msg.AddMsgEvent(MsgID, 20, 714)
			    msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			    msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
			    msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
			    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		    end
		else
			--提示：“您缺少通冥祭魔石，无法选择此项。”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 716)
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 20)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 706)   --“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
    end
end

aux.RegisterCreatureEvent(3021003, 7, "n3021003_OnTalk")


