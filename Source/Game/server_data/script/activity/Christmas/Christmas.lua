--4900713 圣诞老人
--4900714 圣诞树

function Christmas_OnStart(actID)
    Christmas_Creature ={}
	Christmas_Creature[1] = { TypeID=4900713,x=1578,y=14364,z=1970 ,CreID=0}
	Christmas_Creature[2] = { TypeID=4900714,x=1552,y=14325,z=1969 ,CreID=0}
	Christmas_Creature[3] = { TypeID=4900715,x=1824,y=13983,z=1990 ,CreID=0}
	Christmas_Creature[4] = { TypeID=4900715,x=1783,y=14354,z=1942 ,CreID=0}
	Christmas_Creature[5] = { TypeID=4900715,x=1702,y=14553,z=1933 ,CreID=0}
	Christmas_Creature[6] = { TypeID=4900715,x=1648,y=14516,z=1956 ,CreID=0}
	Christmas_Creature[7] = { TypeID=4900715,x=1599,y=14448,z=1975 ,CreID=0}
	Christmas_Creature[8] = { TypeID=4900715,x=1859,y=13983,z=1992 ,CreID=0}
	Christmas_Creature[9] = { TypeID=4900715,x=1839,y=14289,z=1914 ,CreID=0}
	Christmas_Creature[10] = { TypeID=4900715,x=1757,y=14440,z=1894 ,CreID=0}
	Christmas_Creature[11] = { TypeID=4900715,x=1662,y=14543,z=1867 ,CreID=0}
	Christmas_Creature[12] = { TypeID=4900715,x=1603,y=14182,z=1896 ,CreID=0}
	if Christmas == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100301)	--圣诞节活动已经开始，请各位玩家到酆都广场寻找圣诞老人参与活动吧！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		Christmas_Creature[1].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[1].TypeID, Christmas_Creature[1].x, Christmas_Creature[1].y, Christmas_Creature[1].z, 1)
		Christmas_Creature[2].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[2].TypeID, Christmas_Creature[2].x, Christmas_Creature[2].y, Christmas_Creature[2].z, 1)
		Christmas_Creature[3].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[3].TypeID, Christmas_Creature[3].x, Christmas_Creature[3].y, Christmas_Creature[3].z, 1)
		Christmas_Creature[4].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[4].TypeID, Christmas_Creature[4].x, Christmas_Creature[4].y, Christmas_Creature[4].z, 1)
		Christmas_Creature[5].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[5].TypeID, Christmas_Creature[5].x, Christmas_Creature[5].y, Christmas_Creature[5].z, 1)
		Christmas_Creature[6].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[6].TypeID, Christmas_Creature[6].x, Christmas_Creature[6].y, Christmas_Creature[6].z, 1)
		Christmas_Creature[7].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[7].TypeID, Christmas_Creature[7].x, Christmas_Creature[7].y, Christmas_Creature[7].z, 1)
		Christmas_Creature[8].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[8].TypeID, Christmas_Creature[8].x, Christmas_Creature[8].y, Christmas_Creature[8].z, 1)
		Christmas_Creature[9].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[9].TypeID, Christmas_Creature[9].x, Christmas_Creature[9].y, Christmas_Creature[9].z, 1)
		Christmas_Creature[10].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[10].TypeID, Christmas_Creature[10].x, Christmas_Creature[10].y, Christmas_Creature[10].z, 1)
		Christmas_Creature[11].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[11].TypeID, Christmas_Creature[11].x, Christmas_Creature[11].y, Christmas_Creature[11].z, 1)
		Christmas_Creature[12].CreID =  map.MapCreateColCreature(3017299407, -1, Christmas_Creature[12].TypeID, Christmas_Creature[12].x, Christmas_Creature[12].y, Christmas_Creature[12].z, 1)
	end
end
function Christmas_OnEnd(actID)
	if Christmas == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100302)	--圣诞节活动已经结束了,感谢大家的参与。
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		for i = 1 , 12 do
			if Christmas_Creature[i].CreID ~= 0 then
				map.MapDeleteCreature(3017299407, -1, Christmas_Creature[i].CreID)
			end
		end
		Christmas_Creature = nil
	end
end
aux.RegisterActEvent(157, 2, "Christmas_OnStart")
aux.RegisterActEvent(157, 3, "Christmas_OnEnd")
