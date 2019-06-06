#pragma once

namespace Cool3D
{
	class Cool3D_API LockableObject
	{
	public:
		LockableObject(void)	{}
		virtual ~LockableObject(void)	{}

		virtual void Lock() = 0;
		virtual void Unlock() = 0;

	private://nocopyable
		LockableObject(const LockableObject&);
		const LockableObject& operator  = (const LockableObject&);
	};
}//namespace Cool3D