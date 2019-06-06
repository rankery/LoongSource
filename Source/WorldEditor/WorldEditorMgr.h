#pragma once

#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;
#include "..\WorldBase\WorldBaseDefine.h"
using namespace WorldBase;
#include "..\WorldBase\NavMapEditor\NavMapEditor.h"

class WorldEditorMgr
{
public:
	WorldEditorMgr(void);
	~WorldEditorMgr(void);

	static WorldEditorMgr* Inst();
	void Update(float deltaTime);
	void Render();

	TerrainEditor *GetTerrain()	{ return m_pTerrainEditor; }
	NavMapEditor  *GetNavMap()	{ return m_pNavMapEditor; }
	GameMapEditor *GetMap()		{ return m_pMapEditor; }

	void ClearGameMap()	{ m_pMapEditor->ClearEditList(false); }

	void Create();
	void Destroy();


protected:
	TerrainEditor	*m_pTerrainEditor;
	NavMapEditor	*m_pNavMapEditor;
	GameMapEditor	*m_pMapEditor;
};
