
#pragma warning(disable:4251)//Wraning of D3DXVECTOR3 etc need dll export

//--STL
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <map>
using namespace std;

#include <D3D9.h>
#include <d3dx9math.h>
#pragma comment(lib,"d3dx9.lib")

//--

#include "Kernel.h"

#include "VFS\IFS.h"
#include "VFS\StdFS.h"

#include "util\RTTIObj.h"
#include "util\RTTInfo.h"
#include "util\Exception.h"
#include "util\PlugIn.h"
#include "util\ConfigFile.h"
#include "util\FArchive.h"
#include "util\MemPool.h"
#include "util\ObjectPool.h"
#include "util\Filename.h"
#include "util\StlExt.h"
#include "Util\SpeedAnalyser.h"
#include "Util\Timer.h"

#include "Thread\CriticalSection.h"

#include "..\Cool3D\Public\MathBaseDefine.h"
#include "Math\Math.h"
#include "Math\Color.h"
#include "Math\Transform.h"
#include "Math\Frustum.h"
#include "Math\CameraBase.h"
#include "Math\CameraEuler.h"
#include "Math\GeomDefine.h"
#include "Math\ArcBall.h"
#include "Math\Noise.h"
#include "Math\Ray.h"

#include "Image\Image.h"

#include "Device\IDevice.h"
#include "Device\IDeviceTex.h"
#include "Device\IRenderTexture.h"
#include "Device\IFont.h"
#include "Device\IDepthFont.h"
#include "Device\IVertShader.h"
#include "Device\IPixelShader.h"
#include "Device\DrawX.h"
#include "Device\Light.h"
#include "Device\FogDefine.h"
#include "Device\StateBlock.h"
#include "Device\RenderStatus.h"
#include "RenderSys\RenderSys.h"
#include "RenderSys\RenderNode.h"
#include "RenderSys\DynamicMesh.h"
#include "RenderSys\IRenderCallBack.h"

#include "RenderMtl\IMaterial.h"
#include "RenderMtl\IRenderMtl.h"
#include "RenderMtl\MGeneral.h"
#include "RenderMtl\MBitmap.h"
#include "RenderMtl\MSkin.h"
#include "RenderMtl\MTwoLayers.h"
#include "RenderMtl\MRenderTex.h"
#include "RenderMtl\MPreCombineBitmap.h"
#include "RenderMtl\IMtlModifier.h"
#include "RenderMtl\MMUVTrans.h"
#include "RenderMtl\MMFrameBlend.h"
#include "RenderMtl\MMAlphaMask.h"
#include "RenderMtl\MMColorWriteControl.h"
#include "RenderMtl\MMDepthFrame.h"


#include "ResSys\ResMgr.h"
#include "ResSys\ResTexture.h"
#include "ResSys\ResRenderable.h"
#include "ResSys\ResStaticMesh.h"
#include "ResSys\ResSceneMesh.h"
#include "ResSys\ResSkin.h"
#include "ResSys\ResSkeleton.h"
#include "ResSys\ResVertShader.h"
#include "ResSys\ResPixelShader.h"

#include "SceneGraph\SceneGraph.h"
#include "SceneGraph\NullSceneGraph.h"
#include "SceneGraph\HugeQuadTreeSG.h"
#include "SceneGraph\EntityNode.h"
#include "SceneGraph\AvatarNode.h"
#include "SceneGraph\SGSkinNode.h"
#include "SceneGraph\SGAvatarAttachment.h"
#include "SceneGraph\SceneProjector.h"
#include "SceneGraph\SGStaticProjector.h"
#include "SceneGraph\SGLightNode.h"
#include "SceneGraph\SGBladeFlare.h"
#include "SceneGraph\SGSkyBox.h"
#include "SceneGraph\SGSunNode.h"
#include "SceneGraph\SGSunFlare.h"
#include "SceneGraph\SGClothNode.h"

#include "Terrain\Heightmap.h"
#include "Terrain\BitMap.h"
#include "Terrain\ResRiver.h"
#include ".\Cool3DOption.h"

#include "SpecialEffect\SGEffectNode.h"
#include "SpecialEffect\SGAttachableEffectNode.h"
#include "SpecialEffect\EffectAll.h"

#include "Draw2D\IDraw2D.h"
#include "Draw2D\PicFont.h"

#include "Animation\AnimationCtrl.h"
#include "ResSys\AniMsgTable.h"

#include ".\Audio\AudioSystem.h"


#include "Cool3DEngineEditor.h"