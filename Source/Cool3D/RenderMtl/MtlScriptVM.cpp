#include "StdAfx.h"
#include ".\mtlscriptvm.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "..\Util\Exception.h"
#include "..\Util\RTTInfo.h"
#include "..\ResSys\ResMgr.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\IRenderMtl.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderMtl\MtlCombiner.h"
#include "..\terrain\TerrainMtl.h"
#include "..\ResSys\ResTexture.h"

namespace F3D
{
	IMaterial *g_curMtl=NULL;

	//按照制定的class name创建IMaterial对象
	//参数1:字符串,class name
	int mlNewMtl(lua_State *L)
	{
		const char* szClassName=lua_tostring(L,1);
		g_curMtl=(IMaterial*)RTTFactory::Inst()->CreateObj(szClassName);
		ASSERT(g_curMtl!=NULL && "Unknow mtl class");
		return 0;
	}

	//设置TextureBasePath
	//参数1:整数,path类型
	int mlSetTexBasePath(lua_State *L)
	{
		const char* szPath=lua_tostring(L,1);
		ResMgr::Inst()->SetTextureBasePath(szPath);
		return 0;
	}
	//设置当前Bitmap Material的贴图
	//参数1:整数,texture index
	//参数2:字符串,textture name
	int mlMBTexture(lua_State *L)
	{
		UINT index=lua_tonumber(L,1);
		const char* string=lua_tostring(L,2);
		ASSERT(g_curMtl->IS_STATIC_CLASS(MBitmap));
		MBitmap* pMtl=(MBitmap*)g_curMtl;
		pMtl->SetTexture(index,string);
		return 0;
	}
	//设置bitmap Mtl的mip map虑镜
	//参数1:整数,enum ETextrueFilter,see class IDevice
	int mlMBMipFilter(lua_State *L)
	{
		UINT num=lua_tonumber(L,1);
		ASSERT(g_curMtl->IS_STATIC_CLASS(MBitmap));
		MBitmap* pMtl=(MBitmap*)g_curMtl;
		pMtl->SetMipFilter((ETextrueFilter)num);
		return 0;
	}


	class MtlScriptVM::Member
	{
	public:
		lua_State	*m_pLua;

		Member()
		{
			m_pLua = lua_open();
			lua_baselibopen(m_pLua);
			lua_iolibopen(m_pLua);
			lua_strlibopen(m_pLua);
			lua_mathlibopen(m_pLua);

			lua_register(m_pLua, "NewMtl", mlNewMtl);
			lua_register(m_pLua, "SetTexBasePath", mlSetTexBasePath);
			lua_register(m_pLua, "MBTexture", mlMBTexture);
			lua_register(m_pLua, "MBMipFilter", mlMBMipFilter);
			
		}
		~Member()
		{
			if(m_pLua!=NULL)
			{
				lua_close(m_pLua);
				m_pLua=NULL;
			}
		}


	};

	MtlScriptVM::MtlScriptVM(void)
	{
		m_p=new Member;
	}

	MtlScriptVM::~MtlScriptVM(void)
	{
		delete m_p;
	}

	MtlScriptVM* MtlScriptVM::Inst()
	{
		static MtlScriptVM g_mtlScriptVM;//产生对象时会做初始化lua的动作
		return &g_mtlScriptVM;
	}

	void MtlScriptVM::Init()
	{
		//不用做什么
	}

	IMaterial* MtlScriptVM::RunString(const char* szScript)
	{
		ASSERT(szScript!=NULL);

		g_curMtl=NULL;
		ASSERT(m_p!=NULL);
		ASSERT(m_p->m_pLua!=NULL);
		
		lua_dostring(m_p->m_pLua,szScript);

		return g_curMtl;
	}

	
}//namespace F3D