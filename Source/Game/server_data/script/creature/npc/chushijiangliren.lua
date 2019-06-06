--NPC出师奖励人脚本 ID:4900302
--师傅或者徒弟领取出师奖励、师父领取勤武奖励

--NPC出师奖励人脚本对话脚本
function c4900302_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

  --如果是第一次对话
	if TalkIndex == -1 then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuShiJiangLi"], 1)
	end

	local stepindex = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuShiJiangLi"])

	if stepindex == 1 then

		local MsgID = msg.BeginMsgEvent()

		msg.AddMsgEvent(MsgID, 20, 	181)			--“若您或您的徒弟<color=0xffff0000>出师<color=0xfffff7e0>，即可来我这里领取丰厚奖励！此外徒弟使用<color=0xffff0000>精武点数<color=0xfffff7e0>后，会为师门增加<color=0xffff0000>勤武点数<color=0xfffff7e0>值。师父可使用勤武点数在此处换取勤武奖励，换取一次需要消耗勤武点数<color=0xffff0000>3000<color=0xfffff7e0>点！”
		msg.AddMsgEvent(MsgID, 24, 	TargetID)	    --npc id
		msg.AddMsgEvent(MsgID, 21, 	8)			    --“师父领取出师奖励”按钮
		msg.AddMsgEvent(MsgID, 1, 	182)			--“师父领取出师奖励”文本
		msg.AddMsgEvent(MsgID, 21, 	9)			    --“徒弟领取出师奖励”按钮
		msg.AddMsgEvent(MsgID, 1, 	183)			--“徒弟领取出师奖励”文本
		msg.AddMsgEvent(MsgID, 21, 	10)			    --“师父领取师德奖励”按钮
		msg.AddMsgEvent(MsgID, 1, 	184)			--“师父领取师德奖励”文本
		msg.AddMsgEvent(MsgID, 21, 	11)			    --“领取徒弟升级奖励”按钮
		msg.AddMsgEvent(MsgID, 1, 	196)			--“领取徒弟升级奖励”文本

		stepindex = 2                       --前进到第二步

		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息

	elseif stepindex == 2 then

		local BagFreeSize = role.GetBagFreeSize(RoleID)

		if TalkIndex == 8 then							--“师父领取出师奖励”按钮

			local MsgID = msg.BeginMsgEvent()
			local RemindID = 0

			--师傅可用出师奖励次数
			local nRewardNum = role.GetGraduateRewardNum(MapID, InstanceID, RoleID)

			if nRewardNum < 1 then 				--若玩家（已出师弟子数量-领取弟子出师奖励次数=0）

				RemindID = 186			--提示：“您没有已出师的弟子，或者已经领取过弟子出师奖励！”

			else								--通过以上判断后玩家获得即指定奖励。

				--师傅可用出师奖励次数减1
				nRewardNum = nRewardNum - 1
				role.SetGraduateRewardNum(MapID, InstanceID, RoleID, nRewardNum)

				--获得经验
				role.AddRoleExp(MapID, InstanceID, RoleID, 200000)

				--提示：“您已成功领取弟子出师的奖励！”
				RemindID = 188

				--全服跑马灯广播：
				local ServerMsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(ServerMsgID, 100, 189)				        			--“<师父名>由于徒弟已满50级出师，获得20万点经验值的奖励！”
				msg.AddMsgEvent(ServerMsgID, 2, RoleID)		--<师父名>
				msg.DispatchWorldMsgEvent(ServerMsgID)
			end

			msg.AddMsgEvent(MsgID, 20, RemindID)			--提示内容
			msg.AddMsgEvent(MsgID, 21, 4)							--确定按钮
			msg.AddMsgEvent(MsgID, 1, 185)						--“确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)   --发送消息

		elseif TalkIndex == 9 then					--“徒弟领取出师奖励”按钮

			local MsgID = msg.BeginMsgEvent()
			local RemindID = 0

			local GraduateReward = role.GetRoleScriptData(RoleID, 1, RoleDataType["GraduateReward"])

			if GraduateReward ~= 1 then						--若玩家尚未出师，或玩家已领取过出师奖励

				RemindID = 190			--提示：“您尚未出师，或已领取过出师奖励！”

			elseif BagFreeSize < 1 then		--若玩家行囊空格数<1

				RemindID = 187			--提示：“行囊空间不足，请清理后再进行领取！”

			else							--通过以上判断后玩家获得即指定奖励。

			  --标示玩家已领取过徒弟出师奖励
				role.SetRoleScriptData(RoleID, 1, RoleDataType["GraduateReward"], 0)

				--获得经验
				role.AddRoleExp(MapID, InstanceID, RoleID, 1000000)

				--获得十个七杀图的奖励
				role.AddRoleItem(MapID, InstanceID, RoleID, 1350001, 10, 3, 104)

				--提示：“您已成功领取出师的奖励！”
				RemindID = 191

				--全服跑马灯广播：
				local ServerMsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(ServerMsgID, 100, 192)				        --"<徒弟名>由于已满50级出师，获得了100万点经验值的奖励！（拜师后才可获得出师奖励！）"
				msg.AddMsgEvent(ServerMsgID, 2, RoleID)		--<徒弟名>
				msg.DispatchWorldMsgEvent(ServerMsgID)

			end

			msg.AddMsgEvent(MsgID, 20, RemindID)			--提示内容
			msg.AddMsgEvent(MsgID, 21, 4)							--确定按钮
			msg.AddMsgEvent(MsgID, 1, 185)						--“确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)   --发送消息

		elseif TalkIndex == 10 then					--“师父领取勤武奖励”按钮

			local MsgID = msg.BeginMsgEvent()
			local RemindID = 0
			local QinWuPoint = role.GetQinWuPoint(MapID, InstanceID, RoleID)

			if QinWuPoint < 3000 then				--若勤武点数不足3000

				RemindID = 193			--提示：“您的勤武点数不足3000点，无法领取奖励！”

			elseif BagFreeSize < 4 then			--若玩家行囊空格数<4

				RemindID = 187			--提示：“行囊空间不足，请清理后再进行领取！”

			else								    				--通过以上判断后玩家获得即指定奖励。

				--扣除师德点数
				QinWuPoint = QinWuPoint - 3000
				role.SetQinWuPoint(MapID, InstanceID, RoleID, QinWuPoint)

				--获得一个“化功丹”和三个“进阶合成符壹”的奖励
				role.AddRoleItem(MapID, InstanceID, RoleID, 1300100, 1, 3, 104)
				--注意，因为“进阶合成符壹”最大层叠数为1，一次创建三个物品会导致失败，所以每次只能领取一个。
				role.AddRoleItem(MapID, InstanceID, RoleID, 1412004, 1, 3, 104)
				role.AddRoleItem(MapID, InstanceID, RoleID, 1412004, 1, 3, 104)
				role.AddRoleItem(MapID, InstanceID, RoleID, 1412004, 1, 3, 104)

				--提示：“您已成功领取勤武奖励！”
				RemindID = 194

				--全服跑马灯广播：
				local ServerMsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(ServerMsgID, 100, 195)				        			--"<师父名>使用3000点勤武点数换取了化功丹及进阶合成符壹的奖励！"
				msg.AddMsgEvent(ServerMsgID, 2, RoleID)		--<师父名>
				msg.DispatchWorldMsgEvent(ServerMsgID)

			end

			msg.AddMsgEvent(MsgID, 20, RemindID)			--提示内容
			msg.AddMsgEvent(MsgID, 21, 4)							--确定按钮
			msg.AddMsgEvent(MsgID, 1, 185)						--“确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)   --发送消息

		elseif TalkIndex == 11 then                 --"领取徒弟升级奖励"按钮

			local MsgID = msg.BeginMsgEvent()
			local RemindID = 0
			local LevelUpBonus = role.GetLevelUpBonus(MapID, InstanceID, RoleID)

			if LevelUpBonus == nil then
				LevelUpBonus = 0
			end

			if LevelUpBonus < 350 then                        --若徒弟升级积分不足350

				--提示："您当前的徒弟升级积分为<徒弟升级积分>点，需要达到350点才可领取徒弟升级积分奖励！"
				RemindID = 197;

			elseif BagFreeSize < 2 then                    --若玩家行囊空格数<2

				--提示："行囊空间不足，请清理后再进行领取！"
				RemindID = 187

			else                            --通过以上判断后玩家获得即指定奖励

				--获得随机物品
				local ItemID = {1411003, 1411004, 1411005}
				local ItemID2 = {4200008, 4200009, 4200010}
				local ItemID3 = {4200108, 4200109, 4200110}
				local ItemID4 = {4200208, 4200209, 4200210}
				local ItemID5 = {4200308, 4200309, 4200310}
				local ItemID6 = {4200408, 4200409, 4200410}
				local ItemID7 = {4200508, 4200509, 4200510}
				local ItemName = {"先祖之灵符", "神砥之灵符", "众神之灵符"}
				local ItemNum = {3, 2, 1}

				local temp = math.random(100)
				local RandNum = 0

				if temp<=60 then
					RandNum = 1
				elseif temp<=92 then
					RandNum = 2
				else
					RandNum = 3
				end

				temp = math.random(12)
				local ItemID_give = 0
				local ItemNum_give = 0

				if temp<=6 then
					ItemID_give = ItemID[RandNum]
					ItemNum_give = ItemNum[RandNum]
				elseif temp==7 then
					ItemID_give = ItemID2[RandNum]
					ItemNum_give = 1
				elseif temp==8 then
					ItemID_give = ItemID3[RandNum]
					ItemNum_give = 1
				elseif temp==9 then
					ItemID_give = ItemID4[RandNum]
					ItemNum_give = 1
				elseif temp==10 then
					ItemID_give = ItemID5[RandNum]
					ItemNum_give = 1
				elseif temp==11 then
					ItemID_give = ItemID6[RandNum]
					ItemNum_give = 1
				elseif temp==12 then
					ItemID_give = ItemID7[RandNum]
					ItemNum_give = 1
				end

				role.AddRoleItem(MapID, InstanceID, RoleID, ItemID_give, ItemNum_give, 3, 104)

				--获得5个“神匠经”
				role.AddRoleItem(MapID, InstanceID, RoleID, 2005201, 5, 3, 104)

				--提示："您已成功领取徒弟升级积分奖励！"
				RemindID = 198

				--全服跑马灯广播：
				local ServerMsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(ServerMsgID, 100, 199)												--全服跑马灯广播："<师父名>使用350点徒弟升级积分换取了<随机道具名>及神匠经的奖励！"
				msg.AddMsgEvent(ServerMsgID, 2, RoleID)			--<师父名>
				msg.AddMsgEvent(ServerMsgID, 4, ItemID_give)     				--<随机道具名>
				msg.DispatchWorldMsgEvent(ServerMsgID)

				role.SetLevelUpBonus(MapID, InstanceID, RoleID, LevelUpBonus - 350)

			end

			msg.AddMsgEvent(MsgID, 20, RemindID)			--提示内容
    		if RemindID == 197 then
				msg.AddMsgEvent(MsgID, 9, LevelUpBonus)   --<徒弟升级积分>
			end
			msg.AddMsgEvent(MsgID, 21, 4)							--确定按钮
			msg.AddMsgEvent(MsgID, 1, 185)						--“确定”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)   --发送消息

		end

		stepindex = 1			--回到第一步

	end

	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuShiJiangLi"], stepindex)

end

--注册出师奖励人NPC对话事件
aux.RegisterCreatureEvent(4900302, 7, "c4900302_OnTalk")
