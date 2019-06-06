#pragma once

/**
*class ProgressEx
*brief 可以更换前景图片的扩展状态条
*author hyu
*remark
*base GUIProgress
*/
class ProgressEx :
	public GUIProgress
{
public:
	ProgressEx(void);
	~ProgressEx(void);

	//设置前景图片,传入路径文件名
	VOID SetForePic(const tstring strFileName);

	//获取当前前景文件名
	const tstring GetForePic() { return m_strForePicFileName; }
};
