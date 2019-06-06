#pragma once
#include "..\Public\MathBaseDefine.h"

class IFS;
namespace Cool3D
{
	/**	\class EditorDecoFile
		\brief 存储装饰物的位置信息，提供查找、增加、删除装饰物的位置的功能
	*/
	struct tagEditorDecoInfo
	{
		Vector3	pos;
		Vector2	sizeRandom;
		Vector3	rotateRandom;
	};

	class EditorDecoFile
	{
	private:
		struct tagHead
		{
			int numDeco;
		};

		struct tagSRInfo
		{
			float	height;
			Vector2 sizeRandom;
			Vector3	rotateRandom;
		};

		struct DecoNode 
		{
			map<float,tagSRInfo> childMap;
		};

	public:
		EditorDecoFile();
		virtual ~EditorDecoFile();

		bool Save(const TCHAR *szFile);
		void Load(IFS *pFS,const TCHAR *szFile);
		bool GetGrass(const Vector2& leftTop,const Vector2& rightBottom,vector<tagEditorDecoInfo>& out);
		void AddGrass(const Vector2& leftTop,const Vector2& rightBottom,UINT density,float height);
		void DeleteGrass(const Vector2& leftTop,const Vector2& rightBottom,UINT density);
		void ChangeGrassHeight(const Vector2& leftTop,const Vector2& rightBottom,float height);

		bool HaveDeco();
		bool HaveDeco(const Vector2& leftTop,const Vector2& rightBottom);
		void Clear();

	private:
		typedef pair<float,DecoNode*> NodePair;
		typedef pair<float,tagSRInfo> ChildPair;
		typedef map<float,DecoNode*>::iterator NodeIter;
		typedef map<float,tagSRInfo>::iterator ChildIter;

		map<float,DecoNode*>	m_decoNodeMap;
	};
}//namespace Cool3D