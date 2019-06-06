#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class ConfigFile
		\brief ≈‰÷√Œƒº˛
	*/
	class Cool3D_API ConfigFile
	{
	public:
		ConfigFile(const TCHAR* szFileName);
		virtual ~ConfigFile(void);

		double		GetNumber(const TCHAR* szVarName) const;
		int			GetInt(const TCHAR* szVarName) const;
		tstring		GetString(const TCHAR* szVarName) const;
		bool		GetBool(const TCHAR* szVarName) const;
	private:
		tstring		m_szFileName;
	};
}//namespace Cool3D