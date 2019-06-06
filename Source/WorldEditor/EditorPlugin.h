#pragma once

#include "..\Cool3D\Public\IPlugEditor.h"
#include "..\Cool3D\Cool3DEngine.h"
using namespace Cool3D;

class EditorPlugin : public PlugIn
{
public:
	EditorPlugin(void);
	virtual ~EditorPlugin(void);

	void CreateAll(vector<IPlugEditor*>& editors);
	void DeleteAll();

protected:
	vector<IPlugEditor*>	m_created;
};
