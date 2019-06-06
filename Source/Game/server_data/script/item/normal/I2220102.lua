function I2220102_OnUse(MapID, InstanceID, TypeID, TargetID)
	--���ID�� ������ƷTypeID��������Ʒ�ʣ�����ģʽ��log��������Ϣö��
	role.AddRoleItem(MapID, InstanceID, TargetID, 2000027, 1, -1, 8, 420)
end

function I2220102_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--�жϱ������пռ��Ƿ��㹻
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--��ʾ��ұ����ռ䲻��
		bRet = 40
		return bRet, bIgnore
	end

	if 0==role.RemoveFromRole(MapID, InstanceID, TargetID, 2220002, 10, 420) then
		return 0, false
	else
		return 32, false
	end

end

aux.RegisterItemEvent(2220102, 1, "I2220102_OnUse")
aux.RegisterItemEvent(2220102, 0, "I2220102_CanUse")