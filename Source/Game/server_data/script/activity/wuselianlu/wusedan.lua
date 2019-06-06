function I2611635_GiftBag(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleExp(MapID, InstanceID, TargetID, 2500)
end

aux.RegisterItemEvent(2611635, 1, "I2611635_GiftBag")
