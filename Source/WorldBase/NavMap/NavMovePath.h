#pragma once

namespace WorldBase
{
	/**	\class NavMovePath
		\brief 碰撞使用的路径
	*/
	class NavMovePath
	{
	public:
		struct tagMovePoint
		{
			tagMovePoint()
			{}
			tagMovePoint(const Vector3& _pos,float _dist)
				:pos(_pos),dist(_dist)
			{}

			Vector3 pos;
			float dist;
		};

	public:
		NavMovePath(void);
		~NavMovePath(void);

		void Clear(){m_path.clear();}
		void SetStartPos(const Vector3& startPos){m_startPos=startPos;}
		void AddPoint(const Vector3& pos);
		void AddPoint(const Vector3& pos,float dist);

		float GetDist() const;
		void SlerpPos(float factor,Vector3& out) const;
		const Vector3& Back() const;

	private:
		Vector3				m_startPos;
		list<tagMovePoint>	m_path;
	};
}//namespace WorldBase