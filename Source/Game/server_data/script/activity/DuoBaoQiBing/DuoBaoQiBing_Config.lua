------------------------------------------------------------------------------------------------
---------------------------------夺宝奇兵活动配置信息-------------------------------------------
------------------------------------------------------------------------------------------------

BaoXiang_Pos_Broadcast_Interval				= 5			--宝箱位置广播的时间间隔（单位：分钟）
BaoXiang_Update_Interval							= 600		--宝箱位置刷新的时间间隔（单位：秒）

BaoXiang_Hold_Time_Mod						= 0			--0：表示宝箱在不同玩家身上的时间不能累加  1：表示宝箱在不同玩家身上要累加

Active_Min_Level										= 30			--参与活动玩家最低级别

BaoXiang_CDTime										= 1800		--可以开启宝箱的时间（单位：秒）

Activity_Start_Hour										= 21			--开启活动的时间
One_Hour_Before_Start_Hour						= 20			--开启活动的时间的前一个小时
Activity_End_Hour										= 0			--活动结束的时间

Open_Chase_Need_Free_Space = {}							--开启宝箱需要的背包剩余空间
Open_Chase_Need_Free_Space[BaoXiang_ID_M04] = {Free_Space=3}
Open_Chase_Need_Free_Space[BaoXiang_ID_M07] = {Free_Space=3}
Open_Chase_Need_Free_Space[BaoXiang_ID_M107] = {Free_Space=5}

BaoXiang_Pos_MaxCnt = 7
BaoXiang_Pos = {}
BaoXiang_Pos[BaoXiang_ID_M04]   =
{
{x = 2575,	y = 12475,	z = 993},
{x = 1635,	y = 14285,	z = 651},
{x = 1471,	y = 12385,	z = 964},
{x = 1464,	y = 12585,	z = 2024},
{x = 2370,	y = 13040,	z = 1544},
{x = 3060,	y = 22580,	z = 2363},
{x = 2687,	y = 21399,	z = 3318}
}
BaoXiang_Pos[BaoXiang_ID_M07]   =
{
{x = 926	,	y = 17145,	 z = 3697},
{x = 2170,	y = 14318,	 z = 3271},
{x = 973	,	y = 7385,		 z = 2585},
{x = 1164,	y = 9415,		 z = 2020},
{x = 1407,	y = 11650,	 z = 477},
{x = 2598,	y = 7650,	     z = 1330},
{x = 3607,	y = 10350,	 z = 1451}
}
