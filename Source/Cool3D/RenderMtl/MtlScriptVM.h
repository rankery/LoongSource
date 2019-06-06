#pragma once

namespace F3D
{
	class IMaterial;

	/**	\todo 使用序列化代替我
	*/
	class MtlScriptVM
	{
	public:
		MtlScriptVM(void);
		~MtlScriptVM(void);

		static MtlScriptVM* Inst();
		
		IMaterial* RunString(const char* szScript);
	
	private:
		friend class Kernel;

		void Init();
		class Member;
		Member *m_p;
	};
}//namespace F3D