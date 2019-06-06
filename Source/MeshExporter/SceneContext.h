/**	\mainpage 3DS Max8 Mesh Export Plug-in
	
	\section Design
	\par
		设计目标是输出MAX的Mesh, Light数据. 前者应支持Polygon,Triangle两种图元.
		应支持输出大型场景模型(with LightingMap)和简单的Mesh对象(例如武器等).
		并应可作为关键帧动画输出的基础(SceneContext代表一帧??).
	\par
		核心设计使用的设计模式有Strategy,Composition,Singleton. SceneContext是
	Composition的实现, 它调用不同的SceneWriter的具体Strategy写入不同的二进
	制格式文件.	
	\par
		Max中的Mesh无法设计成FlyWight. 虽然所有的Max物体在MAX pipleline 的开始点
		可能是共享的, 但是重要的是此Object要经过复杂的多样的modifier, 才能达到最
		后的World space state, 并不是简单的坐标系变换.
	
	\section Usage
	\par
		将编译生成的"MeshExporter.dle"拷贝到"Max\plugins"目录, 重新启动MAX即可.
	在需要输出是选择MAX菜单:File->Export, 然后选择需要的文件类型即可.
	\par
		如果使用Max生成Lighting Map, 需要选择使用Map channel为3.
		如果使用Max生成Normal Map，需要选择使用Map channel为4.
	\par
		编译此源码需要安装Max8.

	\version 1.0.0
	\author	房燕良 
	\modify [3/23/2008 leili]
	\date	创建于2003年9月29日
*/
#include "Max.h"
#include <vector>
#include "Noncopyable.h"
#include <string>
#include "..\F3DMaxExp\TagNodeExp.h"
#pragma once


//!	\namespace Cool3D
namespace Cool3D
{
	class ExpMesh;
	class ExpLight;
	class ExpMtl;

	/** \class SceneContext
		\brief 输出场景内容的composion, Core class of this plug-in
		\par
		SceneContext设计为Singleton, 为当前对应的MAX场景内容. 它需要保存各种想要输
		出所有MAX的数据. 这些数据要对应需要的MAX的时间状态(ObjectState), 要Writer
		friendly.
		\par
		输出流程应该为:1.Enumerate所有需要输出的INode和Mtl, 并分类保存; 2.将需要输出的
		world space state根据指定的时间构造ExpMesh etc; 3.保存到文件
		\par
		主要保存一个Mesh数组, 一个Light数组.

		\todo 分离出一组特殊的需要交互的场景物件Nodes(可能包含动画), 在输出场景是分别输出
	*/
	class SceneContext  
	{
	public:
		SceneContext();
		~SceneContext();

		/**增加一个新的材质
			\param the mtl,子材质ID
			\return	新建材质的ID
			\remarks 搜索已有材质, 如果没有找到才增加
			\see ExpMesh::MergeFace()
		*/
		unsigned int MergeMtl(Mtl *pMaxMtl,int sub,Mtl *pBakedMtl);

		/**清空所有内部数据
		*/
		void ClearAll();

		/** 得到所有需要输出的MAX节点
			\see SceneEnumProc
		*/
		void BuildNodeList(IScene* pScene);

		/** 将Node List中的MAX节点输出到自定义数据结构
			\param 需要输出的MAX动画的时间点, 默认为动画起始点
			\see BuildNodeList()
		*/
		void ExportData(TimeValue time=0);
	
		/** 将场景写入文件
			\param 文件路径. 使用后缀名来确定存盘文件类型
			\return 是否成功
			\remarks 根据文件类型, 产生不同的Strategy, 并应用它写入文件
			\see SceneWriter, POLWriter
		*/
		bool WriteToFile(const TCHAR* szPath, bool bExpLMUV=false, bool bExpBUUV=false, const TCHAR* szOrigin=NULL);

		INode *GetNode(const TCHAR *szOrigin) const;
		bool GetMat(INode *pNode, D3DXMATRIX& mat) const;
		
		void AddMeshNode(INode* pNode);
		void AddBoneNode(INode* pNode);

		//-------------------------------------
		//内部数据读出接口
		int GetNumMeshs()
		{ return m_pMeshs.size(); }
		ExpMesh* GetMesh(int i)
		{
			//ASSERT(i>=0 && i<m_pMeshs.size());
			return m_pMeshs[i];
		}
		int GetNumMtls()
		{ return m_mtlLib.size(); }
		ExpMtl *GetMtl(int i)
		{
			return m_mtlLib[i];
		}
		TagNodeExp *GetTagNodeExp()
		{
			return &m_tagNodeExp;
		}
		//-------------------------------------

		/** 返回Singleton指针
		*/
		static SceneContext* Inst();
		void ClearExportData();

	private:
		//--MAX数据
		std::vector<INode*>	m_pMeshNodes;
		std::vector<INode*> m_pBoneNodes;
		TagNodeExp			m_tagNodeExp;

		//--导出数据
		std::vector<ExpMtl*>	m_mtlLib;
		std::vector<ExpMesh*>	m_pMeshs;
	};
	
}