---任务随机表
WeiDao_Rate_1_30 = {}
WeiDao_Rate_1_30[1]=21
WeiDao_Rate_1_30[2]=63
WeiDao_Rate_1_30[3]=68
WeiDao_Rate_1_30[4]=76
WeiDao_Rate_1_30[5]=79
WeiDao_Rate_1_30[6]=81
WeiDao_Rate_1_30[7]=83
WeiDao_Rate_1_30[8]=88
WeiDao_Rate_1_30[9]=100
WeiDao_Rate_1_30[10]=100
WeiDao_Rate_1_30[11]=100

WeiDao_Rate_31_99 = {}
WeiDao_Rate_31_99[1]=19
WeiDao_Rate_31_99[2]=56
WeiDao_Rate_31_99[3]=61
WeiDao_Rate_31_99[4]=69
WeiDao_Rate_31_99[5]=72
WeiDao_Rate_31_99[6]=76
WeiDao_Rate_31_99[7]=80
WeiDao_Rate_31_99[8]=85
WeiDao_Rate_31_99[9]=99
WeiDao_Rate_31_99[10]=100
WeiDao_Rate_31_99[11]=100

WeiDao_Rate_101_199 = {}
WeiDao_Rate_101_199[1]=14
WeiDao_Rate_101_199[2]=42
WeiDao_Rate_101_199[3]=49
WeiDao_Rate_101_199[4]=59
WeiDao_Rate_101_199[5]=62
WeiDao_Rate_101_199[6]=68
WeiDao_Rate_101_199[7]=74
WeiDao_Rate_101_199[8]=81
WeiDao_Rate_101_199[9]=97
WeiDao_Rate_101_199[10]=98
WeiDao_Rate_101_199[11]=100

WeiDao_Rate_201_250 = {}
WeiDao_Rate_201_250[1]=9
WeiDao_Rate_201_250[2]=28
WeiDao_Rate_201_250[3]=35
WeiDao_Rate_201_250[4]=46
WeiDao_Rate_201_250[5]=51
WeiDao_Rate_201_250[6]=59
WeiDao_Rate_201_250[7]=67
WeiDao_Rate_201_250[8]=74
WeiDao_Rate_201_250[9]=92
WeiDao_Rate_201_250[10]=95
WeiDao_Rate_201_250[11]=100

WeiDao_Rate_251_299 = {}
WeiDao_Rate_251_299[1]=7
WeiDao_Rate_251_299[2]=21
WeiDao_Rate_251_299[3]=28
WeiDao_Rate_251_299[4]=39
WeiDao_Rate_251_299[5]=44
WeiDao_Rate_251_299[6]=52
WeiDao_Rate_251_299[7]=62
WeiDao_Rate_251_299[8]=69
WeiDao_Rate_251_299[9]=87
WeiDao_Rate_251_299[10]=92
WeiDao_Rate_251_299[11]=100

function weidao_rand_func(LoopNum, RoleLvl)

	local k=math.random(100)
	local quest_type=0
	local Index=0
	local weidao_rand_table = {}

	if RoleLvl > 100 then
	    RoleLvl = 100
	end

	--100,200,300环时给予卫道专属BOSS任务
	if LoopNum==100 or LoopNum==200 or LoopNum==300 then
		Index = 12000+math.floor(RoleLvl/5)-8+LoopNum/50-1
	else
		--否则按照几率给予通常任务
		if LoopNum>=1 and LoopNum<=30 then
			weidao_rand_table=WeiDao_Rate_1_30
		elseif LoopNum>=31 and LoopNum<=99 then
			weidao_rand_table=WeiDao_Rate_31_99
		elseif LoopNum>=101 and LoopNum<=199 then
			weidao_rand_table=WeiDao_Rate_101_199
		elseif LoopNum>=201 and LoopNum<=250 then
			weidao_rand_table=WeiDao_Rate_201_250
		elseif LoopNum>=251 and LoopNum<=299 then
			weidao_rand_table=WeiDao_Rate_251_299
		end

		for i=1, 11 do
			if k<=weidao_rand_table[i] then
				quest_type=i
				break
			end
		end
	end
	--跑腿类任务
	if quest_type==1 then
		if RoleLvl>=40 then
			Index = math.random(1001,1030)
		elseif RoleLvl>=55 then
			Index = math.random(1001,1045)
		elseif RoleLvl>=70 then
			Index = math.random(1001,1053)
		end

	--小怪类任务
	elseif quest_type==2 then
		if RoleLvl>=90 then
			Index = math.random(2051,2061)
		elseif RoleLvl>=80 then
			Index = math.random(2041,2050)
		elseif RoleLvl>=70 then
			Index = math.random(2031,2040)
		elseif RoleLvl>=60 then
			Index = math.random(2021,2030)
		elseif RoleLvl>=50 then
			Index = math.random(2011,2020)
		elseif RoleLvl>=40 then
			Index = math.random(2001,2010)
		end

	--药品、食品类
	elseif quest_type==3 then
		if RoleLvl>=100 then
			Index = math.random(3001,3038)
		elseif RoleLvl>=90 then
			Index = math.random(3001,3030)
		elseif RoleLvl>=80 then
			Index = math.random(3001,3028)
		elseif RoleLvl>=70 then
			Index = math.random(3001,3020)
		elseif RoleLvl>=60 then
			Index = math.random(3001,3018)
		elseif RoleLvl>=50 then
			Index = math.random(3001,3010)
		elseif RoleLvl>=40 then
			Index = math.random(3001,3008)
		end

	--低级材料类
	elseif quest_type==4 then
		if RoleLvl>=90 then
			Index = math.random(4001,4038)
		elseif RoleLvl>=70 then
			Index = math.random(4001,4036)
		elseif RoleLvl>=40 then
			Index = math.random(4001,4028)
		end

	--卫道商品类
	elseif quest_type==5 then
		if RoleLvl>=100 then
			Index = math.random(5019,5020)
		elseif RoleLvl>=90 then
			Index = math.random(5016,5018)
		elseif RoleLvl>=80 then
			Index = math.random(5013,5015)
		elseif RoleLvl>=70 then
			Index = math.random(5010,5012)
		elseif RoleLvl>=60 then
			Index = math.random(5007,5009)
		elseif RoleLvl>=50 then
			Index = math.random(5004,5006)
		elseif RoleLvl>=40 then
			Index = math.random(5001,5003)
		end

	--活动代币类
	elseif quest_type==6 then
		if RoleLvl>=80 then
			Index = math.random(6001,6008)
		elseif RoleLvl>=70 then
			Index = math.random(6001,6007)
		elseif RoleLvl>=60 then
			Index = math.random(6001,6006)
		elseif RoleLvl>=50 then
			Index = math.random(6001,6005)
		elseif RoleLvl>=40 then
			Index = math.random(6001,6004)
		end

	--IM道具类
	elseif quest_type==7 then
		if RoleLvl>=90 then
			Index = math.random(7001,7018)
		elseif RoleLvl>=80 then
			Index = math.random(7001,7017)
		elseif RoleLvl>=70 then
			Index = math.random(7001,7016)
		elseif RoleLvl>=60 then
			Index = math.random(7001,7015)
		elseif RoleLvl>=50 then
			Index = math.random(7001,7014)
		elseif RoleLvl>=40 then
			Index = math.random(7001,7013)
		end

	--高级材料类
	elseif quest_type==8 then
		if RoleLvl>=90 then
			Index = math.random(8001,8019)
		elseif RoleLvl>=70 then
			Index = math.random(8001,8018)
		elseif RoleLvl>=40 then
			Index = math.random(8001,8014)
		end

	--装备类
	elseif quest_type==9 then
		if RoleLvl>=100 then
			Index = math.random(9145,9165)
		elseif RoleLvl>=90 then
			Index = math.random(9121,9144)
		elseif RoleLvl>=80 then
			Index = math.random(9097,9120)
		elseif RoleLvl>=70 then
			Index = math.random(9073,9096)
		elseif RoleLvl>=60 then
			Index = math.random(9049,9072)
		elseif RoleLvl>=50 then
			Index = math.random(9025,9048)
		elseif RoleLvl>=40 then
			Index = math.random(9001,9024)
		end

	--副本BOSS类
	elseif quest_type==10 then
				if RoleLvl>=100 then
			Index = math.random(10014,10015)
		elseif RoleLvl>=90 then
			Index = math.random(10012,10013)
		elseif RoleLvl>=80 then
			Index = math.random(10008,10011)
		elseif RoleLvl>=70 then
			Index = 10007
		elseif RoleLvl>=60 then
			Index = math.random(10005,10006)
		elseif RoleLvl>=50 then
			Index = math.random(10003,10004)
		elseif RoleLvl>=40 then
			Index = math.random(10001,10002)
		end

	--稀有道具类
	elseif quest_type==11 then
		Index = math.random(11001,11015)

	end

	return Index

end
