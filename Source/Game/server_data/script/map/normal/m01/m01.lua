
Marriage_Available = 0

--如果m01被加载，则结婚系统开放，将全局变量Marriage_Available设为1.
function m01_OnInit(MapID,InstanceID,CreatureID,CreatureTypeID)
	Marriage_Available = 1
end
aux.RegisterMapEvent("m01", 0, "m01_OnInit")
