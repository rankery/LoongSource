--与禁军教头3021017对话，传送进演武场。
function n3021017_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
	    --if BiWuQieCuo_Available == 2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2001) -- "禁军教头：\n    本将最看不起的就是那些只会纸上谈兵的家伙，你要想得到本将赏识，就必须在演武场中证明自己的能力！\n    小提示：演武场是供玩家PK打斗之用，进入演武场，除了自己的队友之外，玩家可以攻击所有的其他玩家而不会有任何惩罚。"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			--msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
			--msg.AddMsgEvent(MsgID, 1, 2002) -- "进入80级以下演武场"
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 2003) -- "进入81级以上演武场"
			msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
			msg.AddMsgEvent(MsgID, 1, 2004)   --“下次再说吧”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		--[[else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2021) -- "禁军教头：\n    本将最看不起的就是那些只会纸上谈兵的家伙，你要想得到本将赏识，就必须在演武场中证明自己的能力！\n    小提示：演武场是供玩家PK打斗之用，进入演武场，除了自己的队友之外，玩家可以攻击所有的其他玩家而不会有任何惩罚。\n    (演武场即将开放，敬请期待！)"
			msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end]]
    --[[elseif TalkIndex == 4 then
        local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if level ~=nil then
		    if level >= 30 then
			    if level <= 80 then
					role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092184628, 332, 43, 319)--进入a04
				else
				    local MsgID = msg.BeginMsgEvent()
		            msg.AddMsgEvent(MsgID, 20, 2005) -- "您的等级太高，不能进入低级演武场。"
 		            msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
                    msg.AddMsgEvent(MsgID, 21, 6) -- 确定按钮
                    msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
		            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2006) -- "您的等级太低，不能进入演武场。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, 6) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
        end]]
    elseif TalkIndex == 5 then
        --local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		--if level ~=nil then
		    --if level >= 30 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092184628, 332, 43, 319)--进入a04
			--[[else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2006) -- "您的等级太低，不能进入演武场。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, 6) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
        end]]
    end
end

aux.RegisterCreatureEvent(3021017, 7, "n3021017_OnTalk")


--与禁军教头3021018对话，传送到涿鹿黄帝城。
function n3021018_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2007) -- "禁军教头：\n    想离开这里的时候跟我说一声，本将自会让你出去。"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 2008) -- "离开演武场，回到黄帝城"
		msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 2009)   --“我暂时不想离开”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 2102, 367, 2650)--回到黄帝城
    end
end

aux.RegisterCreatureEvent(3021018, 7, "n3021018_OnTalk")
