--玩家脚本数据定义
--玩家脚本数据一共为100个，后面可按需求扩充
--版本对外之后，已经设定索引的脚本数据不能再换索引

RoleDataType = {}
RoleDataType["Injury"]              = 0		    --内伤治疗状态

RoleDataType["WaBao"]               = 1		    --挖宝任务环数
RoleDataType["WaBao_Total"]         = 2         --挖宝今日总任务环数
RoleDataType["WaBao_Update"]        = 3         --挖宝任务上次更新时间
RoleDataType["WaBao_Index"]         = 4         --当前接到的挖宝任务的Index
RoleDataType["IsDoWaBao"]           = 5         --目前是否接有寻宝类任务

RoleDataType["BangRen"]             = 6         --帮派任务环数
RoleDataType["BangRen_Total"]       = 7	        --帮派任务今日总任务环数
RoleDataType["BangRen_Update"]      = 8         --帮派任务上次更新时间
RoleDataType["BangRen_Index"]       = 9         --当前接到的帮派任务的Index

RoleDataType["ZiCai"]               = 10		--帮派任务环数
RoleDataType["ZiCai_Total"]         = 11		--帮派任务今日总任务环数
RoleDataType["ZiCai_Update"]        = 12		--帮派任务上次更新时间
RoleDataType["ZiCai_Index"]         = 13		--当前接到的帮派任务的Index

RoleDataType["AnDing"]              = 14		--帮派任务环数
RoleDataType["AnDing_Total"]        = 15		--帮派任务今日总任务环数
RoleDataType["AnDing_Update"]       = 16		--帮派任务上次更新时间
RoleDataType["AnDing_Index"]        = 17		--当前接到的帮派任务的Index

RoleDataType["LongFeng"]            = 18		--帮派任务环数
RoleDataType["LongFeng_Total"]      = 19		--帮派任务今日总任务环数
RoleDataType["LongFeng_Update"]     = 20		--帮派任务上次更新时间
RoleDataType["LongFeng_Index"]      = 21		--当前接到的帮派任务的Index
RoleDataType["LongFeng_Cont"]       = 22		--当前接到的帮派任务的Index
RoleDataType["LongFeng_Mark"]       = 23		--当前接到的帮派任务的Index

--24已用，在后面，ChuMo_Index

RoleDataType["RolePoint"]           = 25		-- 答题活动中玩家的总得分数
RoleDataType["RoleMapID"]           = 26        -- 答题活动中玩家所在的地图id
RoleDataType["RoleInstanceID"]      = 27        -- 答题活动中玩家所在的地图instanceid

RoleDataType["IsDoWaBao"]           = 30        --目前是否接有寻宝类任务 0没有 1有
RoleDataType["PositionA"]           = 31        --挖宝地点坐标a
RoleDataType["PositionB"]           = 32        --挖宝地点坐标b
RoleDataType["PositionC"]           = 33        --挖宝地点坐标c
RoleDataType["PositionMap"]         = 34        --挖宝地点所在场景

RoleDataType["IsDoWaBao"]	        = 35        --目前是否接有寻宝类任务 0没有 1有
RoleDataType["Act14_Expire"]        = 36	    --玩家上次参加英雄乱武活动的届数
RoleDataType["Act18_Expire"]        = 37	    --玩家上次参加群雄逐鹿活动的届数
RoleDataType["Act18_TeamKey"]       = 38        --玩家参加群雄乱舞活动时的小队编号

RoleDataType["Shengxiao_Val"]		= 40		--玩家生肖传说的完成记录

RoleDataType["ShiZuRand_Index"]	    = 41        --当前氏族随机任务Index
RoleDataType["ShiZuRand_Loop"]	    = 42        --当前氏族随机任务环数
RoleDataType["ShiZuRand_Total"]	    = 43        --今日氏族随机任务总环数
RoleDataType["ShiZuRand_Update"]    = 44        --氏族随机任务上次更新时间
RoleDataType["IsDoShiZuRand"]	    = 45        --氏族随机任务上次更新时间
RoleDataType["ShiZuRand_Count"]	    = 46        --氏族随机任务总分数
RoleDataType["ShiZuRand_StoneID"]	= 47		--记录玄晶石的ID，用于玩家放弃任务时删除

RoleDataType["ZhuE_UseTimes"]		= 48	--重复任务诛恶物品诛恶密信的使用次数
RoleDataType["ZhuE_Loop"]			= 49	--诛恶任务当前环数
RoleDataType["ZhuE_Total"]			= 50	--诛恶任务总环数
RoleDataType["ZhuE_Update"]			= 51	--诛恶任务上次更新时间，用于检测今日完成次数
RoleDataType["ZhuE_Pos"]			= 52	--诛恶任务随机坐标点

RoleDataType["BingBei_Loop"]		= 53	--兵备任务当前环数
RoleDataType["BingBei_Total"]		= 54	--兵备任务总环数
RoleDataType["BingBei_Update"]		= 55	--兵备任务上次更新时间

RoleDataType["ChuMo_Index"]			= 24	--除魔任务坐标点
RoleDataType["ChuMo_Total"]			= 56	--除魔任务坐标点
RoleDataType["ChuMo_Pos"]			= 57	--除魔任务坐标点
RoleDataType["ChuMo_Mon"]			= 58	--除魔任务坐标点
RoleDataType["ChuMo_Loop"]			= 59	--除魔任务当前环数
RoleDataType["ChuMo_TeamLevel"]		= 60	--除魔任务接取任务时的队伍平均等级
RoleDataType["ChuMo_Update"]		= 61	--除魔任务接取任务时的队伍平均等级

RoleDataType["WeiDao"]				= 62	--卫道任务当前环数
RoleDataType["WeiDao_AccLvl"]		= 63	--卫道任务第一环接取等级
RoleDataType["WeiDao_Index"]		= 64	--当前卫道任务Index

RoleDataType["XiuXing_Loop"]		= 65	--修行任务当前环数
RoleDataType["XiuXing_Total"]		= 66	--修行任务总环数
RoleDataType["XiuXing_Update"]		= 67	--修行任务上次更新时间

RoleDataType["JiShi_Loop"]			= 68	--诛恶任务当前环数
RoleDataType["JiShi_Total"]			= 69	--诛恶任务总环数
RoleDataType["JiShi_Update"]		= 70	--诛恶任务上次更新时间，用于检测今日完成次数
RoleDataType["JiShi_Pos"]			= 71	--诛恶任务随机坐标点

RoleDataType["LongMaiXunBao_Do"]	= 72	--玩家是否触发了龙脉寻宝任务，即同意进入龙脉副本
RoleDataType["LongMaiXunBao_NPC_X"] = 73
RoleDataType["LongMaiXunBao_NPC_Y"] = 74
RoleDataType["LongMaiXunBao_NPC_Z"] = 75

RoleDataType["ShiZuRand_Accept"] 	= 76	--玩家能否接取氏族玄晶任务

RoleDataType["Chest_Rate"]			= 77	--玩家开启宝箱的节点掉率

RoleDataType["PKstate"]		= 78	--玩家进入修炼场前的PK状态

RoleDataType["FristTime_GiveBag"]    = 79    --玩家是否第一次获得12908，12909，12911任务的包裹

RoleDataType["SDYL_Done"]			= 80	-- 完成一轮十殿阎罗活动

RoleDataType["YueYao_Pos"]                      =81    --记录月妖的随机坐标
RoleDataType["YueYaoZhiYing_Date"]                      =82    --记录月妖之影当日是否已经领取
RoleDataType["WeiDao_Exp"]		= 83	--当前卫道任务Index
RoleDataType["YueYaoZhiJing_Date"]                      =84    --记录月妖之镜当日是否已经领取
RoleDataType["FDGJ_QuestType"]                      =85    --记录接取的超度亡魂任务的类型。1为普通经验，2为稍好，3为高经验
RoleDataType["FDGJ_QuestID"]                        =86    --记录接取的超度亡魂任务的Index
RoleDataType["FishingLevel"]                        =87    --记录玩家的钓鱼熟练度等级
RoleDataType["FishingNum"]                     	   	=88    --记录玩家的钓鱼熟练度，升级后置0
RoleDataType["JinRuXiuLianChang"]                   =89    --记录玩家进入修炼场的次数

RoleDataType["CS_FX_TalkIndex"]		= 90	-- 城战的对话步骤
RoleDataType["fangwei"]				= 91	-- 城战任务环数
RoleDataType["fangwei_Update"]		= 92	-- 城战任务更新时间
RoleDataType["YinYangNum"]		= 93	--炼化太极活动中向阴阳太极鼎提交道具次数
RoleDataType["BiWuQieCuo_Kill"]		= 94	--比武切磋中获得的连杀数
RoleDataType["jinjieshili"]		= 95	--记录玩家获得的进阶实力值，副本

RoleDataType["NianHuo_Total"]			= 96	--年货任务总环数
RoleDataType["NianHuo_Update"]		= 97	--年货任务上次更新时间，用于检测今日完成次数

RoleDataType["XSXLC_EnterType"]		= 98	--玩家进入新手修炼场的模式

RoleDataType["jinjieshili_A"]		= 99	--记录玩家获得的进阶实力值,活动
RoleDataType["jinjieshili_Q"]		= 100	--记录玩家获得的进阶实力值，随机和每日任务
RoleDataType["jinjieshili_K"]		= 101	--记录玩家获得的进阶实力值，击杀非白名玩家
RoleDataType["PuTiYe_Num"]			= 102	--记录菩提叶任务每天的完成次数
RoleDataType["PuTiYe_Update"]		= 103	--记录菩提叶任务上次更新时间

RoleDataType["LeiFengMao"]			= 104	--记录雷锋帽的总共获取数量

RoleDataType["GraduateReward"]		= 105	--记录是否可以领取出师奖励

RoleDataType["ChuShiJiangLi"]		= 106	--出师奖励人NPC对话状态
RoleDataType["UseJingWuPoint"]		= 107	--记录玩家是否使用精武点数升级
RoleDataType["HaveGraduate"]		= 108	--徒弟是否领取过出师奖励

RoleDataType["Chest_Step"]			= 109	--玩家开启宝箱的步骤

RoleDataType["TotleTimes_Arena"]    = 110   --玩家参与竞技场的总次数
RoleDataType["WinTimes_Arena"]      = 111   --玩家在竞技场中获得胜利的次数
RoleDataType["TimesToday_Arena"]      = 112   --玩家今天参加竞技场比赛的次数
RoleDataType["Date_Arena"]      = 113        --竞技场专用的今天的日期记录
RoleDataType["RankToday_Arena"]      = 114   --玩家今天获得的武勋

RoleDataType["Salary_Times"]			= 115    --玩家本周已领取工资的次数
RoleDataType["Last_Salary_M"]			= 116    --玩家上次领取工资的月份
RoleDataType["Have_FreePoint"]			= 117	--标记玩家是否已经领取35级免费赠点
RoleDataType["QingMing_Total"]			= 118	--清明活动任务本日完成次数，后世博字帖兑换使用
RoleDataType["QingMing_Update"]			= 119	--清明活动任务上次更新日期，后世博字帖兑换使用
RoleDataType["YuRen_Update"]			= 120	--清明愚人牌上次更新日期
RoleDataType["YuRen_Total"]				= 121	--清明愚人牌近日使用次数

RoleDataType["ShopDate_Arena"]      = 122        --竞技场专用的装备购买记录
RoleDataType["BuyEquip_Arena"]      = 123        --是否买了装备

RoleDataType["XJ_Dati_Date"]      = 124        --记录最后一次答题的时间

RoleDataType["Last_Recv_D"]			= 125		--上一次领取神迹经验奖励时间

RoleDataType["Last_PaoSang_D"]		= 126		--上一次领取跑商日期
RoleDataType["Last_PaoSang_C"]		= 127		-- 洗闻苌倘掌谀诮邮芘苌檀问?

RoleDataType["Last_Instance"]		= 128		--上次领取副本完成奖励的副本id

RoleDataType["Last_JCXL_Day"]		= 129		--上次进入禁城修炼场日期

RoleDataType["FaBaoShiLian"]		= 130		--法宝的相关历练值

RoleDataType["VoteDate"]		= 131		--法宝的相关历练值
RoleDataType["SupportTeam"]		= 132		--法宝的相关历练值
RoleDataType["VoteNum"]			= 133		--法宝的相关历练值
RoleDataType["LastBuyWine"]		= 134		--法宝的相关历练值
RoleDataType["Wine_Fee_A"]		= 135		--法宝的相关历练值
RoleDataType["Wine_Fee_B"]		= 136		--法宝的相关历练值

RoleDataType["FeiSheng_shilian"]		= 137		--飞升相关试炼记录：是否杀掉BOSS：个位，炼器数量：十位百位 熔铸：千位。
RoleDataType["FeiShengDuihuan_times"]           = 138       --玩家兑换材料的次数
RoleDataType["FeiShengDuihuan_Update"]  = 139        --玩家上次兑换材料的时间

RoleDataType["LingPaiLingQu_Update"]           = 140       --玩家上次领取清剿匪类令德时间
RoleDataType["ChengZhuJingYan_Update"]           = 141       --玩家上次领取城主发布经验的时间
RoleDataType["FengXiangXiaoXiao_Update"]           = 142       --玩家上次领取帮派活动经验

RoleDataType["Q20315_Times"]           = 143       --玩家完成20315的次数

RoleDataType["Jidou_Last"]				= 144		--上次参加激斗的时间
RoleDataType["Jidou_Times"]				= 145		--本日参加激斗的次数
RoleDataType["m107_killnum"]			= 146		--m107中击杀怪物的数量
RoleDataType["m107_lastkill"]			= 147		--m107中上次击杀怪物的时间

RoleDataType["YinYuanJie"]			= 148	--记录姻缘结的总共获取数量
RoleDataType["YinYuanJie_Year"]			= 149	--记录姻缘结（雷锋帽）的年份

RoleDataType["Tongxin_last"]			= 150	--记录上次使用同心技能日期

RoleDataType["zhongqiu_yueliangshi"]    = 151   --记录中秋节玩家每日获得月亮石个数

RoleDataType["guoqing_tongmingguiyu"]   = 152   --记录国庆节玩家进入通冥鬼域副本调查物资箱子次数
RoleDataType["guoqingdalibao_geren"]   = 153   --记录国庆节玩家开启国庆大礼包个数
RoleDataType["Guoqing_Date"]    = 154   --上次接国庆任务的时间
RoleDataType["Guoqing_WuZi"]    = 155   --本日完成了几次国庆任务
RoleDataType["ZhenZhuJieRenWu"]    = 156   --105随机任务
RoleDataType["ChongzhuYellow"]	= 157			--黄色暗器重铸记录
RoleDataType["ChongzhuGreen"]	= 158			--绿色暗器重铸记录
RoleDataType["ChongzhuBlue"]	= 159			--蓝色暗器重铸记录
RoleDataType["ChongzhuOrange"]	= 160			--橙色暗器重铸记录
RoleDataType["baiyaota_Update"]    = 161   --百妖塔的上次进入的时间和次数
RoleDataType["Christmas_Box"]	= 162  --记录玩家是否已将空礼盒挂在圣诞树上
RoleDataType["Qingrenjie_Yutu"]	= 163  --记录每个玩家喂养的兔子ID
RoleDataType["Qingrenjie_YutuJiangli"]	= 164  --记录每个玩家的领取奖励次数
RoleDataType["Funvjie_Nvwazhilu"]	= 165  --记录每个玩家的领取奖励次数
RoleDataType["MSJD_Award"]	= 166  --记录每个玩家的领取奖励次数
RoleDataType["MSJD_TotlePoint"]	= 167  --记录每个玩家的领取奖励次数
