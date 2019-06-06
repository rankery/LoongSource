#include "MAX.h"
#pragma once

/**	\class SceneEnumProc
	\brief 枚举所有场景中的Node, 把需要输出的Node,记录到SceneContext
	\see MAX class ITreeEnumProc
*/

class SceneEnumProc : public ITreeEnumProc  
{
public:
	/** 检测是Light node
	*/
	bool IsLightNode(INode *pNode);
	/** 检测是否为需要输出的Mesh node
		\param The node to check
		\return true for need export
		\remarks 此Node将可以转换成Triobj或者Polyobj
	*/
	bool IsMeshNode(INode *pNode);
	/**	Max 指定的回调函数
		\param The node
		\return One of the following values may be returned to control how enumeration continues:<br>
			TREE_CONTINUE	Continue enumerating.<br>
			TREE_IGNORECHILDREN	Don't enumerate the children of this node, but continue enumerating.<br>
			TREE_ABORT Stop enumerating.<br>
	*/
	virtual int callback( INode *node );
	SceneEnumProc();
	virtual ~SceneEnumProc();

};
