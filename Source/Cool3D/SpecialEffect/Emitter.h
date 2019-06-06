#pragma once

namespace F3D
{
	//todo:支持Lod,lod越低发射的元素越少? 某些Emitter(Light?)不工作?
	class Emitter
	{
	public:
		Emitter(void);
		virtual ~Emitter(void);
	};
}//namespace F3D