#pragma once

//---------------------------------------------------------------------------/
//									 中文版									 /
//---------------------------------------------------------------------------/

#ifdef CHINESE_VERSION

#define w_UserName			wxT("用户名:")
#define w_Password			wxT(" 密码  :")
#define w_Login				wxT("登陆")
#define w_Logout			wxT("退出")
#define w_Privilege			wxT("权限")
#define w_PlsWait			wxT("连接中，请稍候……")
#define w_ConnectFailed		wxT("连接失败")
#define w_RemoteTool		wxT("运维工具")
#define w_OpenCloseServer	wxT("开关服务器")
#define w_GMManager			wxT("GM管理")
#define w_GoodServerGroup	wxT("正常服务器组")
#define w_BadServerGroup	wxT("异常服务器组")
#define w_OpenCloseSomeServer	wxT("多区开关服")
#define w_OpenSomeServer	wxT("多区开服")
#define w_CloseSomeServer	wxT("多区关服")
#define w_Select			"选中"
#define w_ServerName		wxT("服务器名称")
#define w_Section			wxT("游戏大区")
#define w_GameWorld			wxT("游戏世界")
#define w_ServerState		wxT("状态")
#define w_Actions			wxT("操作")
#define w_Online			wxT("在线人数")
#define w_MaxOnline			wxT("人数上限")
#define w_SelectAll			wxT("全选")
#define w_UnSelectAll		wxT("取消全选")

#define w_CreateGM			wxT("创建GM")
#define w_NullAccount		wxT("账号不能为空")
#define w_NullPsd			wxT("密码不能为空")
#define w_NullIP			wxT("绑定IP不能为空")
#define w_InvalidIP			wxT("您输入的IP不合法")
#define w_Attention			wxT("警告")
#define w_NoAuthCreateGM	wxT("您当前权限无法创建GM")
#define w_NoAuthDeleteGM	wxT("您当前权限无法删除GM")
#define w_NoAuthBindIP		wxT("您当前权限无法变更GM绑定IP")
#define	w_NoAuthQuery		wxT("您当前权限无法进行查询")
#define	w_NoAuthOpenSome	wxT("您当前权限无法使用多区开服功能")
#define	w_NoAuthCloseSome	wxT("您当前权限无法使用多区关服功能")
#define	w_AskForOpen		wxT("您确认要多区开服吗?")
#define	w_AskForClose		wxT("您确认要多区关服吗?")
#define w_ChangeBindIP		wxT("修改绑定IP")
#define w_DeleteGM			wxT("删除GM")
#define w_GMAccount			wxT("GM账号")
#define w_LoginPsd			wxT("登陆密码")
#define w_IPBinded			wxT("绑定IP")
#define w_CreateSuccessfully	wxT("创建GM成功")
#define w_CreateFailed		wxT("创建GM失败")
#define w_DelSuccessfully	wxT("删除GM成功")
#define w_DelFailed			wxT("删除GM失败")
#define w_AccoutUsed		wxT("账号已存在")
#define w_BadIP				wxT("绑定IP验证失败")
#define w_BindIPSuccess		wxT("修改绑定IP成功")
#define w_BindIPFailed		wxT("修改绑定IP失败")
#define w_GMNotExist		wxT("该GM不存在")

#define w_Error				wxT("错误")
#define w_LoginFailed		wxT("登陆失败")
#define w_BadConnection		wxT("与GMServer的连接断开")


#define w_NoInited			"未初始化完毕"
#define w_BrokenDown		"系统出现故障"
#define w_WorkNormally		"良好"
#define w_ProofError		"验证服务器故障"

#define w_Start				wxT("开启")
#define w_Close				wxT("关闭")
#define w_CloseForced		wxT("强制关闭")
#define w_CloseSuccessfully	wxT("关闭服务器成功")
#define w_CloseFailed		wxT("关闭服务器失败")
#define w_StartSuccessfully	wxT("启动服务器成功")
#define w_StartFailed		wxT("启动服务器失败")

#define w_ConfirmOpen		wxT("开服确认");
#define	w_AskForOpen		wxT("您确定要开启")
#define w_ConfirmClose		wxT("关服确认");
#define	w_AskForClose		wxT("您确定要关闭")
#define	w_Forced			wxT("强制")
#define	w_DoClose			wxT("关闭")

#endif

//---------------------------------------------------------------------------/
//									English									 /
//---------------------------------------------------------------------------/

#define w_UserName			wxT("Account: ")
#define w_Password			wxT("Password:")
#define w_Login				wxT("Sign in")
#define w_Logout			wxT("Sign out")
#define w_Privilege			wxT("Privilege")
#define w_PlsWait			wxT("Connecting...")
#define w_ConnectFailed		wxT("Connection Failed")
#define w_SumShow			wxT("Online Sum Show")
#define w_OpenCloseServer	wxT("Show Page")
#define w_GMManager			wxT("GM Manager")
#define w_GoodServerGroup	wxT("")
#define w_BadServerGroup	wxT("Abnormal Server Group")
#define w_OpenCloseSomeServer	wxT("Open or Close Selected Servers")
#define w_OpenSomeServer	wxT("Open Selected Servers")
#define w_CloseSomeServer	wxT("Close Selected Servers")
#define w_SetSystemCmd		wxT("Send System Command to Selected Servers")
#define	w_NoAuthSend		wxT("You don't have privilege to send system command")
#define	w_AskForSend		wxT("Are you sure to send command to selected servers?")
#define	w_AskForSendQ		wxT("Are you sure to send command to ")
#define w_SystemCmd			wxT("Command:")
#define w_SendCmd			wxT("Send")
#define w_Select			"Select"
#define w_ServerType		wxT("Server Type")
#define w_Section			wxT("Section")
#define w_GameWorld			wxT("Game World")
#define w_ServerState		wxT("Server Status")
#define w_Actions			wxT("Operations")
#define w_Online			wxT("User Online")
#define w_Sum				wxT("Sum")
#define w_SelectAll			wxT("Select All")
#define w_UnSelectAll		wxT("Cancel Select")

#define w_CreateGM			wxT("Create GM Account")
#define w_NullAccount		wxT("Account field cannot be left blank")
#define w_NullPsd			wxT("Password field cannot be left blank")
#define w_NullIP			wxT("IP field cannot be left blank ")
#define w_InvalidIP			wxT("Illegal IP")
#define w_Attention			wxT("Warning")
#define w_NoAuthCreateGM	wxT("You don't have privilege to create GM account")
#define w_NoAuthDeleteGM	wxT("You don't have privilege to delete GM account")
#define w_NoAuthBindIP		wxT("You don't have privilege to modify IP binded")
#define	w_NoAuthQuery		wxT("You don't have privilege to do query")
#define	w_NoAuthOpenSome	wxT("You don't have privilege to open selected servers")
#define	w_NoAuthCloseSome	wxT("You don't have privilege to close selected servers")
#define	w_AskForOpen		wxT("Are you sure to open selected servers?")
#define	w_AskForClose		wxT("Are you sure to close selected servers?")
#define w_ChangeBindIP		wxT("Modify IP Binding")
#define w_DeleteGM			wxT("Delete GM Account")
#define w_GMAccount			wxT("GM Account")
#define w_LoginPsd			wxT("Password")
#define w_IPBinded			wxT("Binding IP")
#define w_CreateSuccessfully	wxT("Create GM account successfully")
#define w_CreateFailed		wxT("Fail to create GM account")
#define w_DelSuccessfully	wxT("Success to delete")
#define w_DelFailed			wxT("Fail to delete")
#define w_AccoutUsed		wxT("This account already exist")
#define w_BadIP				wxT("Illegal IP")
#define w_BindIPSuccess		wxT("Success to modify IP binding")
#define w_BindIPFailed		wxT("Fail to modify IP binding")
#define w_GMNotExist		wxT("This account doesn't exist")

#define w_Error				wxT("Error")
#define w_LoginFailed		wxT("Fail to Sign in")
#define w_BadConnection		wxT("Disconnected from GMserver")


#define w_NoInited			"Uninitialized"
#define w_BrokenDown		"system failure"
#define w_WorkNormally		"Well"
#define w_ProofError		"ProofError"

#define w_Start				wxT("Open")
#define w_Close				wxT("Close")
#define w_CloseForced		wxT("Force Close")
#define w_SetMaxOnline		wxT("..")
#define w_CloseSuccessfully	wxT("Close Successfully")
#define w_CloseFailed		wxT("Fail to close")
#define w_StartSuccessfully	wxT("Start Successfully")
#define w_StartFailed		wxT("Fail to start")

#define w_ConfirmOpen		wxT("Confirm to open ");
#define	w_AskForOpenQ		wxT("Are you sure to open ")
#define w_ConfirmClose		wxT("Confirm to close ");
#define	w_AskForCloseQ		wxT("Are you sure to ")
#define w_ConfirmSend		wxT("Confirm to send command ");
#define	w_Forced			wxT("passively ")
#define	w_DoClose			wxT("close ")

#define w_MaxNum			wxT("Reset Max User Online:")
#define w_SetMaxOK			wxT("Reset")
#define	w_NoAuthChangeMax	wxT("You don't have privilege to change it")
#define	w_AskForChangeMax	wxT("Are you sure to change max user online?")
#define w_ConfirmChangeMax	wxT("Confirm to change ");
#define	w_AskForChangeMaxQ	wxT("Are you sure to change max user online of ")

#define w_UpdateMall			wxT("Update Mall:")
#define w_UpdateMallOK			wxT("Update")
#define	w_NoAuthUpdataMall		wxT("You don't have privilege to update mall.")
#define	w_ErrorType				wxT("You can't do this on ")
#define	w_AskForUpdateMall		wxT("Are you sure to update mall of selected worlds?")
#define w_DoUpdate				wxT("Update Mall of Selected Worlds.")
#define w_ConfirmUpdateMall		wxT("Confirm to update mall ");
#define w_Warning4RType			wxT("Selected server with unexpected type.");
#define	w_AskForUpdateMallQ		wxT("Are you sure to update mall of ")