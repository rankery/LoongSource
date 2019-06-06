#pragma once

#include "Loong_LoginDBCtrl.h"
#include "Loong_LoongDBCtrl.h"
#include "Loong_MergeLogicCtrl.h"
/*
/loong db 统一合并
/1、速度快，耗内存
*/
template<class T>
BOOL TMethod_Loong_ServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl);

/*
/loong db 分开合并 
/1、源1与源2无逻辑关系
/2、速度快，耗内存
/2、上千万的表要用分页合并或特殊处理
*/
template<class T>
BOOL TMethod_Loong_SeparateServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl);

/*
/loong db 利用accountid分用户合并 
/1、速度慢 但节省合并机的内存
*/
template<class T>
BOOL TMethod_Loong_AccountServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl);

/*
/loog db 分页查询合并
/1、要求源1与源2无逻辑关系
/2、速度慢
/3、节约内存
*/
template<class T>
BOOL TMethod_Loong_PagingServerMerge(T , CLoongMergeLogicCtrl &clsMergeLogicCtrl);

/*
/login db 统一合并
*/
template<class T>
BOOL TMethod_Login_ServerMerge(T ,CLoongMergeLogicCtrl &clsMergeLogicCtrl);



#include "Loong_TMethod.cpp"
