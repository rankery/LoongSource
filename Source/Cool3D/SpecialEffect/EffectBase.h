#pragma once
#include "..\Util\RTTIObj.h"
#include "..\Util\FArchive.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Math\CameraBase.h"
#include "..\SceneGraph\SceneGraph.h"

namespace Cool3D
{
	inline Vector3 Deg2Rad3f(Vector3& val)
	{
		Vector3 ret;
		ret.x=_DegToRad(val.x);
		ret.y=_DegToRad(val.y);
		ret.z=_DegToRad(val.z);
		return ret;
	}

	struct Cool3D_API tagMtlEffectDesc//用一个结构体描述,便于编辑
	{
		TCHAR	szTexName[256];
		BYTE	blendType;			// 0disable,1alpha blend,2aditiveblend
		bool	bTestZ;				// 是否进行正常的Z值测试
		int		frmU,
				frmV;				// 一个贴图可以存储m*n帧动画
		float	fps;				// 贴图动画的播放速度
		bool	bPlayOnce;			// 贴图动画只播放一次?

		float   fScrollSpeedU;		// 横向滚动速度
		float   fScrollSpeedV;		// 纵向滚动速度
		BOOL	bHeatHaze;			// 是否热处理
		float   fHeatOffsetFator;	// 扰动系数
		bool    bEnableLight;       // 是否受光照影响
		BYTE    byteReserve[2];
		DWORD	dwReserve[11];

		tagMtlEffectDesc()
		{
			memset(this,0,sizeof(*this));
			bTestZ=true;
			frmU=frmV=1;
			fps=10.0f;
			bPlayOnce=false;

			fHeatOffsetFator = 0.02f;
		}
	};

	class RenderNode;
	class MBitmap;
	class SGEffectNode;
	class MMUVPage;

	/**	\class EffectBase
		\brief 各种特效的基类
	*/
	class Cool3D_API EffectBase : public RTTIObj,public ISerializable
	{
	public:
		static bool s_reloadTex;//编辑器模式下,每次创建材质,都重新Load贴图
		struct tagProp
		{
			int			m_groupID;		//在整个特效中的id
			TCHAR		m_szName[32];	//用户标识名称
			bool		m_bActive;		//如果不active,则不update,不render
			float		m_startTime;	//整个特效运行多久后,自己开始运行
			bool		m_loop;			//是否循环播放
			float		m_closeTime;	//关闭需要多少秒
			int         m_nLOD;         //特效细节等级

			DWORD	dwReserve[6];

			tagProp()
			{
				m_groupID=0;	
				memset(m_szName,0,sizeof(m_szName));
				_tcscpy_s(m_szName,sizeof(m_szName)/sizeof(TCHAR),_T("none"));
				m_bActive=true;
				m_startTime=0;
				m_loop=true;
				m_closeTime=1.5f;
				m_nLOD=0;

				memset(dwReserve,0,sizeof(dwReserve));
			}
		};
	public:
		EffectBase(void);
		virtual ~EffectBase(void);

		//--序列化接口
		virtual void Serialize(FArchive& ar);
		virtual void Deserialize(FArchive& ar);

		//!每个Effect拥有的RenderNode的格式在创建完毕后就不能变了<<==重要
		//!主要是为了Scene Node的mtl好对应起来
		virtual int GetNumRenderNodes() =0;
		virtual RenderNode* GetRenderNode(int i) =0;
		virtual void SetGroupID(int id)=0;
		/** 更新内部数据
			\param 两帧间隔时间,整个特效运行的时间
		*/
		virtual void Update(SceneGraph* pSG,SGEffectNode *pSGNode,CameraBase *pCamera,float deltaTime,float runTime) =0;
		/**	当一个特效元素被手动的从SGEffectNode中移除时,调用
			\remarks 主要用于编辑器,预览窗口关闭时,通知特效元素.那些向SGEffectNode添加child scene node的
			特效元素,此时可以detach child.见EffectMesh
		*/
		virtual void OnDetachFromSGNode(SGEffectNode *pSGNode);

		/**	设置特效在世界中的位置(可以产生移动的效果)
			\remarks 如果Effect attach到某个移动的SG Node上,则Effect的所有元素
				都会在SG Node的object space,如果想让Effect产生的元素(例如残影,
				粒子发射器产生的粒子)出生在world space,而Effect又要移动,则需要使用此函数
		*/
		virtual const TCHAR* GetName() const =0;
		virtual void Rename(const TCHAR* szNewName) =0;
		//!将播放状态设为起始状态
		virtual void ResetPlayState() =0;
		//!开始进入平滑关闭状态
		virtual void Close(float sgNodeRunTime);
		//!是否完全关闭,不可见了
		virtual bool IsClosed();
		/*特效播放时长
			\return 如果可以无限播放，返回-1
		*/
		virtual float GetPlayTime(){ return -1;}
		
		//--
		virtual void CreateMtl(const tagMtlEffectDesc& desc);
		virtual IMaterial* GetMtl();
		tagMtlEffectDesc GetMtlDesc() const	{	return m_mtlDesc;}
		//!
		static IMaterial *GetMtlFallback();

		virtual bool GetBox(AABBox& out) const =0;
		virtual int GetLOD() const =0;
	protected:
		//!	按照desc建立材质,不处理材质动画
		void CreateBasicMtl(const tagMtlEffectDesc& desc);
		//!返回更新后的当前状态序号
		int AdvanceState(int curState,int numState,float curStateLastTime,float& stateTime,bool loop);
		//!
		void UpdateClosingFade(float runTime,float timeLen);
	protected:
		bool	m_bClosing;
		float	m_closeTime;//"关闭状态"启动时,特效SGNode运行了多久
		float	m_closingFade;

		tagMtlEffectDesc	m_mtlDesc;	//存档要用
		MBitmap				*m_pMtl;	//每个Effect都有且仅有一个材质
		static MConstColor	s_defaultMtl;//如果pMtl为空,则使用一个这个

		MMUVPage*           m_mmUVPage;  //更新时要用
		DECL_RTTI(EffectBase);
	};
}//namespace Cool3D