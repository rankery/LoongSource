
function x_RoleFinishTrain(MapID, InstanceID, RoleID, TrialID)

--生产技能类

	if TrialID== 1 then
		local ProSkillNum = 0

		for i=0, 9 do
			if role.IsLearnedSkill(RoleID, 90000+i) ~= nil then
				ProSkillNum=ProSkillNum+1
			end
		end

		if ProSkillNum <2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 3001)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		else
			role.OpenTrain(RoleID, 2)
			return 1
		end

	elseif TrialID== 2 then
		for i=0, 9 do
			local k = role.IsLearnedSkill(RoleID, 90000+i)
			if k ~= nil then
				if k>=2 then
					role.OpenTrain(RoleID, 3)
					return 1
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3002)
		msg.AddMsgEvent(MsgID, 9, 2)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 3 then
		for i=0, 9 do
			local k = role.IsLearnedSkill(RoleID, 90000+i)
			if k ~= nil then
				if k>=3 then
					role.OpenTrain(RoleID, 4)
					return 1
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3002)
		msg.AddMsgEvent(MsgID, 9, 3)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 4 then
		for i=0, 9 do
			local k = role.IsLearnedSkill(RoleID, 90000+i)
			if k ~= nil then
				if k>=4 then
					role.OpenTrain(RoleID, 5)
					return 1
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3002)
		msg.AddMsgEvent(MsgID, 9, 4)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 5 then
		for i=0, 9 do
			local k = role.IsLearnedSkill(RoleID, 90000+i)
			if k ~= nil then
				if k>=5 then
					role.OpenTrain(RoleID, 6)
					return 1
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3002)
		msg.AddMsgEvent(MsgID, 9, 5)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 6 then
		for i=0, 9 do
			local k = role.IsLearnedSkill(RoleID, 90000+i)
			if k ~= nil then
				if k>=6 then
					return 1
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3002)
		msg.AddMsgEvent(MsgID, 9, 6)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--氏族声望类

	elseif TrialID== 7 then
		for i=0, 7 do
			if role.GetClanRepValue(RoleID, i)>= 3600 then
				role.OpenTrain(RoleID, 8)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3003)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 8 then
		for i=0, 7 do
			if role.GetClanRepValue(RoleID, i)>= 10800 then
				role.OpenTrain(RoleID, 9)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3004)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 9 then
		for i=0, 7 do
			if role.GetClanRepValue(RoleID, i)>= 22800 then
				role.OpenTrain(RoleID, 10)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3005)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 10 then
		for i=0, 7 do
			if role.GetClanRepValue(RoleID, i)>= 40800 then
				role.OpenTrain(RoleID, 11)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3006)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 11 then
		for i=0, 7 do
			if role.GetClanRepValue(RoleID, i)>= 76800 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3007)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--灵兽培养类

	elseif TrialID== 12 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=1 then
				role.OpenTrain(RoleID, 13)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 1)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 13 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=2 then
				role.OpenTrain(RoleID, 14)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 2)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 14 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=3 then
				role.OpenTrain(RoleID, 15)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 3)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 15 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=4 then
				role.OpenTrain(RoleID, 16)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 4)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 16 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=5 then
				role.OpenTrain(RoleID, 17)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 5)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 17 then
		local petlevel = role.GetPetStepGrade(RoleID)
		if petlevel ~= nil then
			if petlevel>=6 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3008)
		msg.AddMsgEvent(MsgID, 9, 6)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--装备强化类

	elseif TrialID== 18 then
		local PosyTimes = role.GetFstBoxWpnPosyTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=4 then
				role.OpenTrain(RoleID, 19)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3009)
		msg.AddMsgEvent(MsgID, 9, 4)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 19 then
		local PosyTimes = role.GetFstBoxWpnPosyTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=8 then
				role.OpenTrain(RoleID, 20)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3009)
		msg.AddMsgEvent(MsgID, 9, 8)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 20 then
		local PosyTimes = role.GetFstBoxWpnPosyTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=12 then
				role.OpenTrain(RoleID, 21)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3009)
		msg.AddMsgEvent(MsgID, 9, 12)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 21 then
		local PosyTimes = role.GetFstBoxWpnPosyTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=16 then
				role.OpenTrain(RoleID, 22)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3009)
		msg.AddMsgEvent(MsgID, 9, 16)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 22 then
		local PosyTimes = role.GetFstBoxWpnPosyTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=20 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3009)
		msg.AddMsgEvent(MsgID, 9, 20)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 23 then
		local PosyTimes = role.GetFstBoxWpnEngraveTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=4 then
				role.OpenTrain(RoleID, 24)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3010)
		msg.AddMsgEvent(MsgID, 9, 4)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 24 then
		local PosyTimes = role.GetFstBoxWpnEngraveTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=8 then
				role.OpenTrain(RoleID, 25)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3010)
		msg.AddMsgEvent(MsgID, 9, 8)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 25 then
		local PosyTimes = role.GetFstBoxWpnEngraveTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=12 then
				role.OpenTrain(RoleID, 26)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3010)
		msg.AddMsgEvent(MsgID, 9, 12)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 26 then
		local PosyTimes = role.GetFstBoxWpnEngraveTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=16 then
				role.OpenTrain(RoleID, 27)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3010)
		msg.AddMsgEvent(MsgID, 9, 16)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 27 then
		local PosyTimes = role.GetFstBoxWpnEngraveTimes(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=20 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3010)
		msg.AddMsgEvent(MsgID, 9, 20)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 28 then
		local PosyTimes = role.GetFstBoxWpnEquipHoleCnt(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=3 then
				role.OpenTrain(RoleID, 29)
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3011)
		msg.AddMsgEvent(MsgID, 9, 3)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 29 then
		local PosyTimes = role.GetFstBoxWpnEquipHoleCnt(RoleID)

		if PosyTimes~= nil then
			if PosyTimes>=5 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3011)
		msg.AddMsgEvent(MsgID, 9, 5)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--等级类
	elseif TrialID== 78 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=10 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 10)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 79 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=20 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 20)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 80 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=30 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 30)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 30 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=40 then
			role.OpenTrain(RoleID, 31)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 40)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 31 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=50 then
			role.OpenTrain(RoleID, 32)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 50)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 32 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=60 then
			role.OpenTrain(RoleID, 33)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 60)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 33 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=70 then
			role.OpenTrain(RoleID, 34)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 70)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 34 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=80 then
			role.OpenTrain(RoleID, 35)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 80)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 35 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=90 then
			role.OpenTrain(RoleID, 36)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 90)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 36 then
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if Level>=100 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3012)
		msg.AddMsgEvent(MsgID, 9, 100)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--侠义门类

	elseif TrialID== 37 then
		local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])

		if LoopNum>=30 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3013)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 38 then
		local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["WeiDao"])

		if LoopNum>=100 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3014)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--帮派类

	elseif TrialID== 39 then
		local GuildID = guild.GetRoleGuildID(RoleID)

		if GuildID~=4294967295 then
			role.OpenTrain(RoleID, 40)
			role.OpenTrain(RoleID, 41)
			role.OpenTrain(RoleID, 42)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3015)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==40 then
		local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ZiCai"])

		if LoopNum>= 20 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3016)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==41 then
		local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["AnDing"])

		if LoopNum>= 20 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3017)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==42 then
		local Level = role.GetGuildExploit(RoleID)

		if Level>=360 then
			role.OpenTrain(RoleID, 43)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3018)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==43 then
		local Level = role.GetGuildExploit(RoleID)

		if Level>=1080 then
			role.OpenTrain(RoleID, 44)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3019)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==44 then
		local Level = role.GetGuildExploit(RoleID)

		if Level>=3240 then
			role.OpenTrain(RoleID, 45)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3020)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==45 then
		local Level = role.GetGuildExploit(RoleID)

		if Level>=16200 then
			role.OpenTrain(RoleID, 46)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3021)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID==46 then
		local Level = role.GetGuildExploit(RoleID)

		if Level>=113400 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3022)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--副本类

	elseif TrialID== 47 then --无回洞窟
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 22)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 48 then --裕祥山麓
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 23)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 49 then --枭吴祭坛
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 24)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 50 then --礼天殿前
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 26)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 51 then --冰封魔窟
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 25)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 52 then --礼天殿后
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 27)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 53 then --通冥鬼域
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 60)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 54 then --嫘祖之谷
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 28)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 55 then --桃源秘境
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 61)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 56 then --魔王殿壹
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 62)

		if Is_Finished==true then
			role.OpenTrain(RoleID, 57)
			role.OpenTrain(RoleID, 58)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 57 then --魔王殿贰
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 63)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 58 then --魔王殿叁
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 64)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 59 then --昆仑秘道
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 88)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 60 then --青绝洞府
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 89)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 61 then --天墉地宫
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 86)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 62 then --玉虚峰
		local Is_Finished = role.IsRoleHaveTitle(RoleID, 87)

		if Is_Finished==true then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3027)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--竞技场类

	elseif TrialID== 63 then
		local Wuxun = role.GetRoleAttValue(MapID, InstanceID, RoleID, 79)

		if Wuxun>= 500 then
			role.OpenTrain(RoleID, 64)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3023)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 64 then
		local Wuxun = role.GetRoleAttValue(MapID, InstanceID, RoleID, 79)

		if Wuxun>= 3000 then
			role.OpenTrain(RoleID, 65)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3024)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 65 then
		local Wuxun = role.GetRoleAttValue(MapID, InstanceID, RoleID, 79)

		if Wuxun>= 15000 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3025)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

--修炼场类

--66~70

--师徒类

--71~72

--装备类

	elseif TrialID== 73 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200208+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 60)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 74 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200308+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 70)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 75 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200408+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 80)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 76 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200508+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 90)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 77 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200608+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 100)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 77 then
		for i=1, 8 do
			local k = role.GetRoleItemNum(RoleID, 8200608+i)
			if k>=1 then
				return 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3026)
		msg.AddMsgEvent(MsgID, 9, 100)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	--法宝相关试炼

	elseif TrialID== 81 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k % 10 >= 2 then
		    role.OpenTrain(RoleID, 82)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3040)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 82 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k % 10 >= 3 then
		    role.OpenTrain(RoleID, 83)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3041)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 83 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k % 10 >= 4 then
		    role.OpenTrain(RoleID, 84)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3042)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 84 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k % 10 == 5 then
		    role.OpenTrain(RoleID, 85)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3043)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 85 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k > 10 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3044)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	--[[elseif TrialID== 86 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"]) % 10
		if k ==2 or k== 3 then
		    role.OpenTrain(RoleID, 87)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3045)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0]]

	elseif TrialID== 87 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"]) % 1000
		if k >= 10 then
		    role.OpenTrain(RoleID, 93)
		    role.OpenTrain(RoleID, 88)
			role.OpenTrain(RoleID, 90)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3046)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 88 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"]) % 1000
		if k >= 60 then
		    role.OpenTrain(RoleID, 89)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3047)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

    elseif TrialID== 89 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"]) % 1000
		if k >= 120 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3048)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 90 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
		if k >= 1000 then
		    role.OpenTrain(RoleID, 91)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3049)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 91 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
		if k >= 2000 then
		    role.OpenTrain(RoleID, 92)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3050)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 92 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
		if k >= 3000 then
			role.OpenTrain(RoleID, 94)
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3051)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 93 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"]) % 10
		if k ==1 or k== 3 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3052)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	elseif TrialID== 94 then

		local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
		if k >= 4000 then
			return 1
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3053)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0

	end

end

aux.RegisterRoleEvent(28, "x_RoleFinishTrain")
