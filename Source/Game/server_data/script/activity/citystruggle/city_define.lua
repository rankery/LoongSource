-- 禁城卫兵的位置
city_guard = {}
city_guard[1] = {}
city_guard[2] = {}
city_guard[3] = {}
city_guard[4] = {}
city_guard[5] = {}

-- 门对应的怪物和模型ID
city_door = {}
city_door[1] = { CreatureID = 4900019, ModelID = 4900035 }
city_door[2] = { CreatureID = 4900020, ModelID = 4900036 }
city_door[3] = { CreatureID = 4900021, ModelID = 4900037 }
city_door[4] = { CreatureID = 4900022, ModelID = 4900038 }
city_door[5] = { CreatureID = 4900023, ModelID = 4900039 }
city_door[6] = { CreatureID = 4900024, ModelID = 4900040 }
city_door[7] = { CreatureID = 4900025, ModelID = 4900041 }
city_door[8] = { CreatureID = 4900026, ModelID = 4900042 }

-- 活动id对应禁城id，地图id，卫兵塑像数量，卫兵位置table的索引，城战进入坐标table的索引，防卫等级（决定门的类型），门的objID（3个城门）
city_id = {}
city_id[70] = { CityID = 3, MapID = 3695619387, GuardNum = 0, GuardIndex = 1, EnterIndex = 1, DefenceLevel = 0}
city_id[43] = { CityID = 2, MapID = 102, GuardNum = 0, GuardIndex = 2, EnterIndex = 2, DefenceLevel = 0}
city_id[44] = { CityID = 3, MapID = 103, GuardNum = 0, GuardIndex = 3, EnterIndex = 3, DefenceLevel = 0}
city_id[45] = { CityID = 4, MapID = 104, GuardNum = 0, GuardIndex = 4, EnterIndex = 4, DefenceLevel = 0}
city_id[46] = { CityID = 5, MapID = 105, GuardNum = 0, GuardIndex = 5, EnterIndex = 5, DefenceLevel = 0}

-- 各禁城城战进入坐标
Enter_Pos = {}
Enter_Pos[1] = {}
Enter_Pos[1][1] = {x = 808 , y = 10350 , z = 575}
Enter_Pos[1][2] = {x = 684 , y = 10369 , z = 550}
Enter_Pos[1][3] = {x = 920 , y = 10403 , z = 713}
Enter_Pos[1][4] = {x = 528 , y = 10350 , z = 708}
Enter_Pos[1][5] = {x = 530 , y = 10350 , z = 568}
Enter_Pos[1][6] = {x = 337 , y = 8424 , z = 297}
Enter_Pos[1][7] = {x = 305 , y = 8345 , z = 249}
Enter_Pos[1][8] = {x = 314 , y = 8345 , z = 222}
Enter_Pos[1][9] = {x = 335 , y = 8345 , z = 198}
Enter_Pos[1][10] = {x = 356 , y = 8345 , z = 197}

Enter_Pos[2] = {}
Enter_Pos[2][1] = {x = 1, y = 2, z = 3}
Enter_Pos[2][2] = {x = 1, y = 2, z = 3}
Enter_Pos[2][3] = {x = 1, y = 2, z = 3}
Enter_Pos[2][4] = {x = 1, y = 2, z = 3}
Enter_Pos[2][5] = {x = 1, y = 2, z = 3}
Enter_Pos[2][6] = {x = 1, y = 2, z = 3}
Enter_Pos[2][7] = {x = 1, y = 2, z = 3}
Enter_Pos[2][8] = {x = 1, y = 2, z = 3}
Enter_Pos[2][9] = {x = 1, y = 2, z = 3}
Enter_Pos[2][10] = {x = 1, y = 2, z = 3}

Enter_Pos[3] = {}
Enter_Pos[3][1] = {x = 1, y = 2, z = 3}
Enter_Pos[3][2] = {x = 1, y = 2, z = 3}
Enter_Pos[3][3] = {x = 1, y = 2, z = 3}
Enter_Pos[3][4] = {x = 1, y = 2, z = 3}
Enter_Pos[3][5] = {x = 1, y = 2, z = 3}
Enter_Pos[3][6] = {x = 1, y = 2, z = 3}
Enter_Pos[3][7] = {x = 1, y = 2, z = 3}
Enter_Pos[3][8] = {x = 1, y = 2, z = 3}
Enter_Pos[3][9] = {x = 1, y = 2, z = 3}
Enter_Pos[3][10] = {x = 1, y = 2, z = 3}

Enter_Pos[4] = {}
Enter_Pos[4][1] = {x = 1, y = 2, z = 3}
Enter_Pos[4][2] = {x = 1, y = 2, z = 3}
Enter_Pos[4][3] = {x = 1, y = 2, z = 3}
Enter_Pos[4][4] = {x = 1, y = 2, z = 3}
Enter_Pos[4][5] = {x = 1, y = 2, z = 3}
Enter_Pos[4][6] = {x = 1, y = 2, z = 3}
Enter_Pos[4][7] = {x = 1, y = 2, z = 3}
Enter_Pos[4][8] = {x = 1, y = 2, z = 3}
Enter_Pos[4][9] = {x = 1, y = 2, z = 3}
Enter_Pos[4][10] = {x = 1, y = 2, z = 3}

Enter_Pos[5] = {}
Enter_Pos[5][1] = {x = 1, y = 2, z = 3}
Enter_Pos[5][2] = {x = 1, y = 2, z = 3}
Enter_Pos[5][3] = {x = 1, y = 2, z = 3}
Enter_Pos[5][4] = {x = 1, y = 2, z = 3}
Enter_Pos[5][5] = {x = 1, y = 2, z = 3}
Enter_Pos[5][6] = {x = 1, y = 2, z = 3}
Enter_Pos[5][7] = {x = 1, y = 2, z = 3}
Enter_Pos[5][8] = {x = 1, y = 2, z = 3}
Enter_Pos[5][9] = {x = 1, y = 2, z = 3}
Enter_Pos[5][10] = {x = 1, y = 2, z = 3}

-- 凤翔传出坐标
FX_Out_Pos = {}
FX_Out_Pos[1] = {x = 890, y = 225, z = 2307}
FX_Out_Pos[2] = {x = 542, y = 8818, z = 1223}
FX_Out_Pos[3] = {x = 505, y = 8740, z = 1253}
FX_Out_Pos[4] = {x = 565, y = 8740, z = 1276}
FX_Out_Pos[5] = {x = 2535, y = 13100, z = 1618}
FX_Out_Pos[6] = {x = 2557, y = 13100, z = 1577}
FX_Out_Pos[7] = {x = 2516, y = 13100, z = 1571}
FX_Out_Pos[8] = {x = 2269, y = 10968, z = 2467}
FX_Out_Pos[9] = {x = 2255, y = 10968, z = 2422}
FX_Out_Pos[10] = {x = 2232, y = 10968, z = 2473}
FX_Out_Pos[11] = {x = 743, y = 10000, z = 2049}
FX_Out_Pos[12] = {x = 932, y = 10000, z = 1941}
