#pragma once
#include "..\Cool3D.h"

namespace Cool3D
{
	class Heightmap;
	/**	\class BrushOpRecorder
		\brief ¼ÇÂ¼»­Ë¢²Ù×÷
	*/
	class Cool3D_API BrushOpRecorder
	{
	public:
		enum EBrushOpMode
		{
			EBOM_SetVal=0,
			EBOM_AddVal
		};

	private:
		struct tagBrushOp
		{
			EBrushOpMode	brushOpMode;
			int	x;
			int	z;
			int	undoVal;
			int	redoVal;
		};
		typedef std::vector<tagBrushOp> ARRAYBRUSHOP;

	public:
		BrushOpRecorder();
		virtual ~BrushOpRecorder();

		void AddOp(EBrushOpMode mode, int x, int z, int undoVal, int redoVal);
		void EndOpRecord();
		void Clear();

		void UndoOp(Heightmap *pHMap);
		void RedoOp(Heightmap *pHMap);
		
		bool CanUndo();
		bool CanRedo();

		void DrawInfo(int x,int y);

	private:
		class Member;
		Member	*m_p;
	};
}//namespace Cool3D