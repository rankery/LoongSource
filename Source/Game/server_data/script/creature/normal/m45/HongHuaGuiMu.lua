--击杀10只花妖后刷出
--红花鬼母（1010528）
--872, 4348, 340,
--赤魁花妖（1010510）
--X=839 Y=4296 Z=321
--X=835 Y=4296 Z=334
--寒冬树精（1010509）
--X=851 Y=4312 Z=319
--X=846 Y=4306 Z=341
--寒冬树精（1010507）
--X=874 Y=4340 Z=333
--X=872 Y=4348 Z=340

function c1010528_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,4)+2
	map.SetMapScriptData(MapID,InstanceID,1,4,Boss_num)
	if Boss_num%4 == 3 then
		map.MapCreateColCreature(MapID, InstanceID, 1010529, 418, 6101, 795, 1, "")
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1776)	--青狄怨魂已经出现于坐标[418, 795]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)     --发送消息
	end
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 14)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1777)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end
aux.RegisterCreatureEvent(1010528, 4, "c1010528_OnDie")
