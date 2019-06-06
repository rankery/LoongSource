#pragma once
#include "..\Cool3D\Cool3DEngine.h"
#include "..\Cool3D\vfs\ifs.h"
#include "..\Cool3D\SpecialEffect\SGAttachableEffectNode.h"
using namespace Cool3D;

class Cool3DModels
{
public:
	class	EffectProperty
	{
	public:
		SGAttachableEffectNode 	* m_Effect; 
		tstring					m_EffectPath;
		tstring					m_AttachPointName;
	
		EffectProperty()
		{
			m_Effect = new Cool3D::SGAttachableEffectNode;
		}

		~EffectProperty()
		{
			if(m_Effect)
				delete m_Effect;
		}
	};

protected:
	class UsefulMember
	{
	public:
		CameraEuler		m_cameraEuler;
		ArcBall			m_arcBall;
		SceneGraph	*	m_pSceneGraph;
		bool			m_created;
		EntityNode	*	m_pEntity;
		tstring			m_EntityPath;
		AvatarNode	*	m_pAvatar;
		tstring			m_AvatarPath;
		SGLightNode *   m_pTestLgt;

		list<EffectProperty> m_EffectList;

		UsefulMember();
		~UsefulMember();
		SceneNode* GetActiveNode();
		Vector3 GetActiveNodeCenter();
		void ResetCamera();
	public:
		bool TRACEEffectPropertyListCounter(const TCHAR * functionName);
	};

public:
	Cool3DModels(void);
	~Cool3DModels(void);

	UsefulMember m_UsefulMember;

	bool initialEngine(HWND iRenderHWND);
	bool loadModel(const TCHAR * iModelFilePath);

	static Cool3DModels * Inst(void)
	{
		static Cool3DModels m_ModelInstance;
		return & m_ModelInstance;
	}
	void render(void);
	AnimationCtrl* GetSceneNodeAni();
	SceneGraph* GetSceneGraph();
	ArcBall* Cool3DModels::GetArcBall()
	{
		return &m_UsefulMember.m_arcBall;
	}
	SceneNode* GetSceneNode()
	{
		if(m_UsefulMember.m_pEntity)
			return m_UsefulMember.m_pEntity;
		else if(m_UsefulMember.m_pAvatar)
			return m_UsefulMember.m_pAvatar;
		return NULL;
	}
	CameraEuler* Cool3DModels::GetCamera()
	{
		return &m_UsefulMember.m_cameraEuler;
	}
	bool AttachEffect(const TCHAR * iEffectPath, const TCHAR * iAttachPointName);
	bool AttachEffectThroughFileContent(tstring * iEffectAttachingFileContent);
	bool DetachEffect(const TCHAR * iEffectFilePath);
	bool ApplicationClose(void);

	void SetSelectedNode(const TCHAR* szSelNode);

private:
	UINT m_selNode;
};
