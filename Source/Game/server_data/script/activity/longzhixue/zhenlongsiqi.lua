--可否使用函数
function zhenlongsiqi_CanUse(MapID, InstanceID, TypeID, TargetID)

	local allowed_map={}
	allowed_map[1] = 3017298127
	allowed_map[2] = 3017298383
	allowed_map[3] = 3017299663
	allowed_map[4] = 3017299919
	allowed_map[5] = 3017299151
	allowed_map[6] = 1146339967
	allowed_map[7] = 3017299407
	allowed_map[8] = 2983744463
	allowed_map[9] = 2291722097
	allowed_map[10] = 2291721329
	allowed_map[11] = 2291721585
	allowed_map[12] = 2291720817
	allowed_map[13] = 2291721073
	allowed_map[14] = 2291720305
	allowed_map[15] = 2291720561
	allowed_map[16] = 3695619387
	allowed_map[17] = 2092184628
	allowed_map[18] = 2092184884
	allowed_map[19] = 2092185140

    local bRet, bIgnore = 0, false

	for i =1, 19 do

		if MapID==allowed_map[i] then
			return bRet, bIgnore
		end
	end

	return 43, bIgnore
end

aux.RegisterItemEvent(3400001, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400002, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400003, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400004, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400011, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400012, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400013, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400014, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400021, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400022, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400023, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400024, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400031, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400032, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400033, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400034, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400041, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400042, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400043, 0, "zhenlongsiqi_CanUse")
aux.RegisterItemEvent(3400044, 0, "zhenlongsiqi_CanUse")
