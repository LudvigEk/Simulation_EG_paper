//
//	sethelpers.h	-- Routines for reading gene set definitions
//

#ifndef GENESETHELPERS_H
#define GENESETHELPERS_H

#include "types/vector.h"
#include "types/string.h"
#include "types/tablefun.h"

bool Sets_ScanCAT(const CTableIndex &idx_sco, int nAnnotRows_sco, int nKeyCol_sco,
				  const CTableIndex &idx_set, int nAnnotRows_set, int nKeyCol_set, const CVector<Table_SortItem> &vSI_set,
				  CVector<CString> &vGeneSetName, CVector< CVector<int> > &vGeneSet, int c);

bool Sets_ScanGMT(const CTableIndex &idx_sco, int nAnnotRows_sco, int nKeyCol_sco,
				  const CTableIndex &idx_set, int nAnnotRows_set, int nKeyCol_set,
				  CVector<CString> &vGeneSetName, CVector< CVector<int> > &vGeneSet);

bool Sets_SaveGMT(const char *aFilename, 
				  const CTableIndex &idx_sco, int nAnnotRows_sco, int nKeyCol_sco, 
				  const CVector<CString> &vGeneSetName, const CVector< CVector<int> > &vGeneSet);

bool Sets_ScanAndTranslateGMT(const CTableIndex &idx_sco, int nAnnotRows_sco,
							  const CTableIndex &idx_set, int nAnnotRows_set,
							  CVector<CString> &vGeneSetName, CVector< CVector<int> > &vGeneSet);

bool Sets_CheckIdentifiers(const CTableIndex &idx_sco, int nAnnotRows_sco, int nKeyCol_sco);

/*
bool Sets_Init_idx_sco(const CTableIndex &idx_sco, int &nAnnotRows, int &nAnnotCols,
					   CString sKeyCol, int &nKeyCol, 
					   CString sScoreCol, int &nScoreCol);
*/

void Sets_CreateKey(const CTableIndex &idx, int nAnnotRows, int nKeyCol, CVector<Table_SortItem> &vSI);

CLoadBuf *Sets_Load_sco(const char *aFilename, 
						CTableIndex &idx_sco, int &nAnnotRows_sco, int &nAnnotCols_sco, 
						CString sKeyCol_sco, int &nKeyCol_sco, 
						CString sScoreCol_sco, int &nScoreCol_sco, 
						CVector<Table_SortItem> &vKey_sco, CMatrix<float> &m_Data_sco);

CLoadBuf *Sets_Load_set(const char *aFilename,
						CTableIndex &idx_set, int &nAnnotRows_set, int &nAnnotCols_set, 
						int &nKeyCol_set, CVector<Table_SortItem> &vKey_set);

#endif