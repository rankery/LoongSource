#include "StdAfx.h"
#include ".\SceneEditor.h"

namespace Cool3D
{
	SceneEditor::SceneEditor(void)
	{
	}

	SceneEditor::~SceneEditor(void)
	{
	}

	void SceneEditor::Update(float deltaTime)
	{}

	void SceneEditor::Render()
	{}

	SceneNode* SceneEditor::AddStaticNode(const TCHAR* szResName)
	{
		return NULL;
	}

	void SceneEditor::RemoveStaticNode(SceneNode *pNode)
	{}

	SceneNode* SceneEditor::PickNode(POINT cursorPt)
	{
		return NULL;
	}
}//namespace Cool3D