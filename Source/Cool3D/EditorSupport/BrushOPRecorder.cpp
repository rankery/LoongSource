#include "stdafx.h"
#include "BrushOPRecorder.h"
#include "..\Util\DebugMisc.h"
#include "..\Terrain\Heightmap.h"
#include "..\Device\DrawX.h"

namespace Cool3D
{
	class BrushOpRecorder::Member
	{
	public:
		list<ARRAYBRUSHOP*>	m_arrayUndoBrushAct;	//记录撤销动作
		list<ARRAYBRUSHOP*>	m_arrayRedoBrushAct;	//记录重做动作
		ARRAYBRUSHOP		*m_arrayOp;

	public:
		Member()
		{
			m_arrayOp=NULL;
		}

		~Member()
		{
			Clear();
		}

		void ClearActions( list<ARRAYBRUSHOP*>& arrayAct )
		{
			for(list<ARRAYBRUSHOP*>::iterator iter=arrayAct.begin();
				iter!=arrayAct.end();++iter)
			{
				ARRAYBRUSHOP* arrayBrushOp=*iter;
				SAFE_DELETE(arrayBrushOp);
			}
			arrayAct.clear();
		}

		void Clear()
		{
			SAFE_DELETE(m_arrayOp);
			ClearActions(m_arrayUndoBrushAct);
			ClearActions(m_arrayRedoBrushAct);
		}
	};

	BrushOpRecorder::BrushOpRecorder()
	{
		m_p=new Member;
	}

	BrushOpRecorder::~BrushOpRecorder()
	{
		SAFE_DELETE(m_p);
	}

	void BrushOpRecorder::AddOp(EBrushOpMode mode, int x, int z, int undoVal, int redoVal)
	{
		if(m_p->m_arrayOp==NULL)
			m_p->m_arrayOp=new ARRAYBRUSHOP;

		tagBrushOp brushOp;
		brushOp.brushOpMode=mode;
		brushOp.x=x;
		brushOp.z=z;
		brushOp.undoVal=undoVal;
		brushOp.redoVal=redoVal;
		m_p->m_arrayOp->push_back(brushOp);

		if(m_p->m_arrayOp->size()>5000)
		{
			EndOpRecord();
		}
	}

	void BrushOpRecorder::EndOpRecord()
	{
		if(m_p->m_arrayOp==NULL)
			return;
		if (m_p->m_arrayOp->size()>0)
		{
			if(m_p->m_arrayUndoBrushAct.size()>30)
			{
				ARRAYBRUSHOP* arrayBrushOp=*m_p->m_arrayUndoBrushAct.begin();
				SAFE_DELETE(arrayBrushOp);
				m_p->m_arrayUndoBrushAct.erase(m_p->m_arrayUndoBrushAct.begin());
			}
			m_p->m_arrayUndoBrushAct.push_back(m_p->m_arrayOp);
			m_p->m_arrayOp=NULL;
			m_p->ClearActions(m_p->m_arrayRedoBrushAct);
		}
	}

	void BrushOpRecorder::Clear()
	{
		m_p->Clear();
	}

	void BrushOpRecorder::UndoOp( Heightmap *pHMap )
	{
		ASSERT(pHMap!=NULL);

		if (m_p->m_arrayUndoBrushAct.size()>0)
		{
			list<ARRAYBRUSHOP*>::iterator endIter=m_p->m_arrayUndoBrushAct.end();
			--endIter;
			ARRAYBRUSHOP* parrayBrushOp=*endIter;
			m_p->m_arrayUndoBrushAct.erase(endIter);

			ARRAYBRUSHOP::iterator iter=parrayBrushOp->begin();
			for (;iter!=parrayBrushOp->end();++iter)
			{
				switch (iter->brushOpMode)
				{
				case EBOM_SetVal:
					{
						pHMap->SafeSetVal(iter->x,iter->z,iter->undoVal);
					}
					break;
				case EBOM_AddVal:
					{
						pHMap->SafeAddValue(iter->x,iter->z,iter->undoVal);
					}
					break;
				}
			}
			m_p->m_arrayRedoBrushAct.push_back(parrayBrushOp);
		}
	}

	void BrushOpRecorder::RedoOp( Heightmap *pHMap )
	{
		ASSERT(pHMap!=NULL);

		if (m_p->m_arrayRedoBrushAct.size()>0)
		{
			list<ARRAYBRUSHOP*>::iterator endIter=m_p->m_arrayRedoBrushAct.end();
			endIter--;
			ARRAYBRUSHOP* parrayBrushOp=*endIter;
			m_p->m_arrayRedoBrushAct.erase(endIter);

			ARRAYBRUSHOP::iterator iter=parrayBrushOp->begin();
			for (;iter!=parrayBrushOp->end();++iter)
			{
				switch (iter->brushOpMode)
				{
				case EBOM_SetVal:
					{
						pHMap->SafeSetVal(iter->x,iter->z,iter->redoVal);
					}
					break;
				case EBOM_AddVal:
					{
						pHMap->SafeAddValue(iter->x,iter->z,iter->redoVal);
					}
					break;
				}
			}
			m_p->m_arrayUndoBrushAct.push_back(parrayBrushOp);
		}
	}

	bool BrushOpRecorder::CanUndo()
	{
		return m_p->m_arrayUndoBrushAct.size()>0;
	}

	bool BrushOpRecorder::CanRedo()
	{
		return m_p->m_arrayRedoBrushAct.size()>0;
	}

	void BrushOpRecorder::DrawInfo( int x,int y )
	{
		TCHAR szInfo[512];
		int opNum=0;
		if(m_p->m_arrayOp!=NULL)
			opNum=m_p->m_arrayOp->size();
		_stprintf(szInfo,_T("Op=%d,Undo=%d,Redo=%d"),opNum,m_p->m_arrayUndoBrushAct.size(),m_p->m_arrayRedoBrushAct.size());
		DrawX::DrawDebugString(szInfo,x,y);
	}
}//namespace Cool3D