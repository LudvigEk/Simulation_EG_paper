/*******************************************************************************
 *
 *   tableindex.cpp -- 
 *
 *   Björn Nilsson, 2004-2008
 */

#include "tableindex.h"
#include "stringfun.h"
#include "types/matrix.h"

/******************************************************************************/

void CTableIndexRow::Extend(int nNewCols, const char *aDummy)
{
	ASSERT(nNewCols>=0);
	if (nNewCols<0)
		return;
	ASSERT(m_vFirst.GetSize()==m_vLast.GetSize());
	int nOldCols= m_vFirst.GetSize();
	for (int i=nOldCols;i<nNewCols;i++)
	{
		m_vFirst.Add(aDummy);
		m_vLast.Add(aDummy);
	}
}

/******************************************************************************/

bool CTableIndex::IsEmpty() const
{
	if (!GetRowCount())
		return true;
	return !m_vRows[0].m_vFirst.GetSize();
}

bool CTableIndex::IsRectangular() const
{
	// Returns true if table has the same number of elements on all rows.
	int min, max;
	GetColCount(min, max);
	return (min==max);
}

void CTableIndex::GetColCount(int &min, int &max) const
{
	// Returns smallest and largest number of elements per indexed row.
	// The two will co-incide for rectangular (most common case) matrices.
	if (IsEmpty())
	{
		min= max= 0;
		return;
	}

	min= max= m_vRows[0].GetColumnCount();
	for (int i=GetRowCount();--i>0;)
	{
		int j= m_vRows[i].GetColumnCount();
		if (j<min)
			min= j;
		else if (j>max)
			max= j;
	}
}

bool CTableIndex::CreateSubset(CMatrix<std::pair<const char *, const char *>> matrix, const CTableIndex &source)
{
	Clear();
	if(matrix.GetNRows()==0)
	{
		printf("Cannot create empty subset\n");
		return false;
	}

	/***Add annotation rows/cols***/
	this->m_vRows.Add(source.GetRow(0));				//Add annotation row NOTE: to be done for several/specified/detected annotation
	/******************************/
	
	for(int y=0;y<matrix.GetNRows();y++)				//Add all other rows
	{
		CTableIndexRow tir;
		
		for (int x=0;x<matrix.GetNCols();x++)
		{
			tir.m_vFirst.Add(matrix.GetAt(y,x).first);
			tir.m_vLast.Add(matrix.GetAt(y,x).second);
		}
		
		this->m_vRows.Add(tir);
	}
	
	/****Unused class members but copied for completeness****/
	this->m_cSeparator= source.GetSeparatorChar();
	this->m_cQuote= source.GetQuoteChar();
	/********************************************************/

	return true;
}

bool CTableIndex::ScanCSV(const char *ach, PARSECALLBACK pCallback)
{
	// Create table index from comma-separated values (CSV) format

	Clear();

	CTableIndexRow tir;
	int r= 0;
	while (*ach)
	{
		int c=0;
		while (true)
		{
			while (*ach==32 || *ach==9)
				ach++;
			const char *ach0;
			const char *ach1;
			if (*ach=='\"')
			{
				// Quoted element
				ach0= ++ach;
				while (*ach && *ach!='\"') // EOL allowed inside quotes in CSV format
					ach++;
				ach1= ach;

				// TODO: Handle embedded "" 

				if (*ach!='\"')
					return false; // EOF inside quotes
				ach++;
			}
			else
			{
				// Non-quoted element
				ach0= ach;
				while (*ach && *ach!=10 && *ach!=13 && *ach!=',')
					ach++;
				ach1= ach;
			}

			// Ignore leading and trailing spaces
			while (ach0<ach && (*ach0==32 || *ach0==9))
				ach0++;
			while (ach0<ach1 && (*(ach1-1)==32 || *(ach1-1)==9))
				ach1--;

			if (c<tir.m_vFirst.GetSize())
			{
				tir.m_vFirst.SetAt(c, ach0);
				tir.m_vLast.SetAt(c, ach1);
			}
			else
			{
				tir.m_vFirst.Add(ach0);
				tir.m_vLast.Add(ach1);
			}
			c++;

			while (*ach==32 || *ach==9)
				ach++;
			if (*ach==0 || *ach==10 || *ach==13)
				break;
			if (*ach!=',')
				return false;
			ach++;
		}

		tir.m_vFirst.SetSize(c);
		tir.m_vLast.SetSize(c);
		m_vRows.Add(tir);

		if (pCallback)
			pCallback(r);
		r++;
		ach= Scan_SkipLine(ach);
	}

	return true;
}


bool CTableIndex::ScanTDS(const char *ach, PARSECALLBACK pCallback)
{
	// Create table index from tab-delimited, non-quoted data
	// CVector<const char*> vFirst;
	// CVector<const char*> vLast;

	Clear();
	CTableIndexRow tir;
	int r= 0;
	while (*ach)
	{
		int c=0;
		while (true) 
		{
			// Delimit element
			const char *ach0= ach;
			while (*ach && *ach!=9 && *ach!=10 && *ach!=13)
				ach++;
		
			// Ignore leading and trailing spaces
			while (ach0<ach && *ach0==32)
				ach0++;
			const char *ach1= ach;
			while (ach0<ach1 && *(ach1-1)==32)
				ach1--;

			// Remove quotes and spaces immediately inside the quotes
			if (ach0+1<ach1 && *ach0=='\"' && *(ach1-1)=='\"')
			{
				ach0++;	ach1--;
				while (ach0<ach && *ach0==32)
					ach0++;
				const char *ach1= ach;
				while (ach0<ach1 && *(ach1-1)==32)
					ach1--;
			}
			if (c<tir.m_vFirst.GetSize())
			{
				tir.m_vFirst.SetAt(c, ach0);
				tir.m_vLast.SetAt(c, ach1);
			}
			else
			{
				tir.m_vFirst.Add(ach0);
				tir.m_vLast.Add(ach1);
			}
			c++;

			if (*ach==9)
				ach++;
			else if (*ach==0 || *ach==10 || *ach==13)
				break;
		} 


		tir.m_vFirst.SetSize(c);
		tir.m_vLast.SetSize(c);

		m_vRows.Add(tir);
		if (pCallback)
			pCallback(r);
		r++;
		ach= Scan_SkipLine(ach);
	}
	return true;
}

bool CTableIndex::Transpose(CTableIndex &dst)
{
	int R= GetRowCount();
	int C= GetMinColCount();

	try
	{
		dst.m_vRows.SetSize(C);
		for (int c=0;c<C;c++)
		{
			CTableIndexRow tir;
			tir.m_vFirst.SetSize(R);
			tir.m_vLast.SetSize(R);

			for (int r=0;r<R;r++)
			{
				tir.m_vFirst.SetAt(r, GetFirstPointer(r, c));
				tir.m_vLast.SetAt(r, GetLastPointer(r, c));
			}

			dst.m_vRows.SetAt(c, tir);
		}
	}
	catch(...)
	{
		return false; // Out of memory.
	}

	return true;
}

void CTableIndex::MakeRectangular()
{
	int cmax= GetMaxColCount();
	for (int r=0;r<GetRowCount();r++)
		if (GetColCount(r)<cmax)
		{
			CTableIndexRow tir;
			tir= m_vRows.GetAt(r);
			tir.Extend(cmax, GetFirstPointer(0,0));
			m_vRows.SetAt(r, tir);
		}

	ASSERT(IsRectangular());
}

void CTableIndex::Clear()
{
	m_vRows.SetSize(0);
}

/******************************************************************************/

bool CTableIndex::CreateResorted(const CTableIndex &ti, int nAnnotRows, CVector<int> vOrder)
{
	// Creates an index the body of consists of the rows in ti 
	// arranged in the order given in vOrder, except for  the 
	// first m_nAnnotRows rows which are copied as they are.

	m_vRows.SetSize(nAnnotRows+vOrder.GetSize());

	// Annot rows
	for (int i=0;i<nAnnotRows;i++)
		m_vRows.SetAt(i, ti.m_vRows[i]);

	// Reordered body
	for (int i=0;i<vOrder.GetSize();i++)
	{
		int src= vOrder[i];
		if (src<0 || src>=ti.GetRowCount()-nAnnotRows)
		{
			ASSERT(false);
			return false;
		}

		m_vRows.SetAt(i + nAnnotRows, ti.m_vRows[src + nAnnotRows]);
	}

	return true;
}
