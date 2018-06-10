/************************************************************

  ArrayEx.h

  "CArrayEx", a template class that does exactly what "CArray"
  does except that:

  a) No need to link MFC libraries, can be used in any non-MFC
     applications.
  b) Introduced member function "Sort", which sorts stored
     elements using the "quick sort" algorithm(requiring
	 operators ">" being defined for stored element data type.
  c) Added member function "Find", which searches for a given
     element and returns result index(-1 if not found).
  d) The constant operator[] now returns "const TYPE&" instead of
     a "TYPE" instance, which was the case in "CArray".
  e) Overloaded operator "=".

  Special Note: The "QuickSort" method was developed by Martin Ziacek.
 
  2002-03-20		Initial release.
  2002-06-11		Changed "__QuickSortRecursive" member function into
					static so that operator ">" need not be defined if
					member "Sort" is not called.
  
  Bin Liu, abinn32@yahoo.com
  All rights reserved.

*************************************************************/

#ifndef __CARRAYEX_H__
#define __CARRAYEX_H__

#include <assert.h>

template <class TYPE, class ARG_TYPE>
class CArrayEx
{
public:

	// Constructions
	CArrayEx();
	virtual ~CArrayEx();

	// Searching and sorting
	int Find(ARG_TYPE tData, int nStartIndex = 0) const;
	int Find(const TYPE* segment, int nSegmentSize, int nStartIndex = 0) const;
	int Find(const CArrayEx& segment, int nStartIndex = 0) const;
	void Sort(bool bAscending = true);
	
	// Attributes
	int GetSize(void) const	{ return m_nElementCount; }
	int GetUpperBound(void) const { return m_nElementCount - 1; }
	void SetSize(int nNewSize);
	bool IsEmpty() const { return m_nElementCount == 0; }
	bool IsIndexValid(int nIndex) const { return nIndex >= 0 && nIndex < m_nElementCount; }

	// Operations
	void FreeExtra(void);
	void RemoveAll(void) { m_nElementCount = 0; }

	// Element Access
	const TYPE& GetAt(int nIndex) const;
	void SetAt(int nIndex, ARG_TYPE tData);
	TYPE& ElementAt(int nIndex);
	const TYPE* GetData() const { return m_pData; }
	TYPE* GetData()	{ return m_pData; }

	// Growing the Array
	void SetAtGrow(int nIndex, ARG_TYPE tData);
	int Add(ARG_TYPE tData);
	int Append(const CArrayEx& src);
	void Copy(const CArrayEx& src);

	// Insertion/Removal
	void InsertAt(int nIndex, ARG_TYPE tData, int nCount = 1);
	void InsertAt(int nStartIndex, const CArrayEx* pNewArray);
	void RemoveAt(int nIndex, int nCount = 1);

	// Operators
	TYPE& operator[](int nIndex) { return ElementAt(nIndex); }
	const TYPE& operator[](int nIndex) const { return GetAt(nIndex); }
	const CArrayEx& operator=(const CArrayEx& src);

protected:

	// Protected members for internal call only.

	// right shifts elements
	void __RightShift(int nStartAt, int nCount = 1);

	// dynamically grow array size
	void __GrowSize(int nAllocSize =-1);

	// quick sort algorithm
	static void __QuickSortRecursive(TYPE* pArray, int nLow, int nHigh, bool bAscending);

	// initial array size and growing multiplier
	enum { __INIT_SIZE = 32, __GROW_RATE = 2 };

	// member data
	int m_nElementCount; // element count
	int m_nAllocSize; // allocated size
	TYPE* m_pData; // data
};

// the "quick sort" method developed by Martin Ziacek
template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::__QuickSortRecursive(TYPE* pArray, int nLow, int nHigh, bool bAscending)
{
	// recursive quick sort method, by Martin Ziacek.
	int i,j;
	TYPE str;

	i = nHigh;
	j = nLow;

	str = pArray[((int) ((nLow+nHigh) / 2))];

	do {

		if (bAscending) {
			while (str > pArray[j]) j++;
			while (pArray[i] > str) i--;
		} else {
			while (pArray[j] > str) j++;
			while (str > pArray[i]) i--;
		}

		if (i >= j) {

			if ( i != j ) {
				TYPE zal;

				zal = pArray[i];
				pArray[i] = pArray[j];
				pArray[j] = zal;
			}
			i--;
			j++;
		}
	} while (j <= i);

	if (nLow < i) __QuickSortRecursive(pArray, nLow,i,bAscending);
	if (j < nHigh) __QuickSortRecursive(pArray, j,nHigh,bAscending);
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::Sort(bool bAscending)
{
	if (m_nElementCount > 1)
	{
		int low = 0, high = m_nElementCount - 1;
		__QuickSortRecursive(m_pData, low, high, bAscending);
	}
}

template <class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::CArrayEx()
{
	m_nElementCount = 0;
	m_nAllocSize = 0;
	m_pData = 0;	
};

template <class TYPE, class ARG_TYPE>
CArrayEx<TYPE, ARG_TYPE>::~CArrayEx()
{
	if (m_pData != 0)
		delete [] m_pData;
};

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::SetSize(int nNewSize)
{
	if (nNewSize < 0 || nNewSize == m_nElementCount) return;
	
	if (nNewSize > m_nElementCount && nNewSize <= m_nAllocSize)
	{
		m_nElementCount = nNewSize;
		return;
	}

	__GrowSize(nNewSize);
	m_nElementCount = nNewSize;
};

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Add(ARG_TYPE tData)
{
	// append at the end
	InsertAt(m_nElementCount, tData, 1);
	return m_nElementCount - 1;
}


template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Find(ARG_TYPE tData, int nStartIndex/*=0*/) const
{
	if (m_nElementCount == 0)
		return -1;

	assert(IsIndexValid(nStartIndex));
	
	for (int i = nStartIndex; i < m_nElementCount; i++)
	{
		if (m_pData[i] == tData) 
		{
			return i;
		}
	}
	return -1;
}


template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Find(const TYPE* segment, int nSegmentSize, int nStartIndex /*=0*/) const
{
	assert(IsIndexValid(nStartIndex));
	bool bFind = false;

	for (int i = nStartIndex; i <= m_nElementCount - nSegmentSize; i++)
	{
		for (int j = 0, bFind = true; j < nSegmentSize; j++)
		{
			if (segment[j] != m_pData[i + j])
			{
				// not match, stop right here
				bFind = false;
				break;
			}
		}

		if (bFind)
			return i; // the first match position
	}

	return -1;
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Find(const CArrayEx& segment, int nStartIndex/*=0*/) const
{
	return Find(segment.GetData(), segment.GetSize(), nStartIndex);
}

// insert single element
template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE tData, int nCount)
{
	assert(nIndex >= 0 && nCount >= 0);

	int nNewSize = nCount;
	if (nIndex < m_nElementCount)
		nNewSize += m_nElementCount;
	else
		nNewSize += nIndex;

	if (nNewSize > m_nAllocSize)
        __GrowSize(nNewSize);
	
	if (nIndex < m_nElementCount)
		__RightShift(nIndex, nCount);

	for (int i = 0; i < nCount; i++)
		m_pData[nIndex + i] = tData;	

	m_nElementCount = nNewSize;
}

// insert a whole array
template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::InsertAt(int nStartIndex, const CArrayEx* pNewArray)
{
	assert(nStartIndex >= 0);
	const int nCount = pNewArray->GetSize();
	const int nNewSize = nStartIndex + nCount + 1;

	if (nNewSize > m_nAllocSize)
        __GrowSize(nNewSize);
	
	if (nStartIndex < m_nElementCount)
		__RightShift(nStartIndex, nCount);

	for (int i = 0; i < nCount; i++)
		m_pData[nStartIndex + i] = (*pNewArray)[i];	

	m_nElementCount = nNewSize;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
	assert(IsIndexValid(nIndex));
	if (nCount <= 0)
		return;

	nCount = (m_nElementCount - nIndex > nCount) ? nCount : (m_nElementCount - nIndex);
	const int nSegmentSize = m_nElementCount - nIndex - nCount;

	if (nSegmentSize >= 0)
	{
		for (int i = 0; i < nSegmentSize; i++)
		{
			m_pData[nIndex + i] = m_pData[nIndex + i + nCount];
		}
	}

	m_nElementCount -= nCount;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::FreeExtra()
{
	if (m_nAllocSize == m_nElementCount)
		return;

	// reallocate data array with optimized size
	m_nAllocSize = (m_nElementCount < __INIT_SIZE) ? __INIT_SIZE : m_nElementCount;
	TYPE* p = new TYPE[m_nAllocSize];

	for (int i = 0; i < m_nElementCount; i++)
		p[i] = m_pData[i];

	delete [] m_pData;
	m_pData = p;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::Copy(const CArrayEx& src)
{
	m_nElementCount = src.m_nElementCount;
	m_nAllocSize = src.m_nAllocSize;
	
	if (m_pData != 0)
		delete [] m_pData;

	m_pData = new TYPE[m_nAllocSize];

	for (int i = 0; i < m_nElementCount; i++)
		m_pData[i] = src.m_pData[i];
}

template <class TYPE, class ARG_TYPE>
int CArrayEx<TYPE, ARG_TYPE>::Append(const CArrayEx& src)
{
	int nRet = src.m_nElementCount > 0 ? m_nElementCount : -1;
	__GrowSize(m_nElementCount + src.m_nElementCount);

	// append at the end
	for (int i = 0; i < src.m_nElementCount; i++)
		m_pData[m_nElementCount++] = src.m_pData[i];
	
	return nRet;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::__RightShift(int nStartAt, int nCount /*= 1*/)
{
	if (m_nElementCount == 0 || nCount <= 0)
		return;

	 assert(IsIndexValid(nStartAt));

	 for (int i = m_nElementCount - 1; i >= nStartAt; i--)
		 m_pData[i + nCount] = m_pData[i];		 
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::__GrowSize(int nAllocSize /*=-1*/)
{
	// determine the new allocate array size
	if (m_nAllocSize == 0)
		m_nAllocSize = __INIT_SIZE;
	do
	{
		m_nAllocSize *= __GROW_RATE;
	} while (m_nAllocSize < nAllocSize);

	TYPE* p = new TYPE[m_nAllocSize];
	for (int i = 0; i < m_nElementCount; i++)
		p[i] = m_pData[i];

	if (m_pData != 0)
		delete [] m_pData;
	m_pData = p;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::SetAt(int nIndex, ARG_TYPE tData)
{
	assert(IsIndexValid(nIndex));
	m_pData[nIndex] = tData;
}

template <class TYPE, class ARG_TYPE>
void CArrayEx<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE tData)
{
	assert(nIndex >= 0);
	if (nIndex < m_nElementCount)
		m_pData[nIndex] = tData;
	else
		InsertAt(nIndex, tData, 1);
}

template <class TYPE, class ARG_TYPE>
TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{
	assert(IsIndexValid(nIndex));
	return m_pData[nIndex];
}

template <class TYPE, class ARG_TYPE>
const TYPE& CArrayEx<TYPE, ARG_TYPE>::GetAt(int nIndex) const
{
	assert(IsIndexValid(nIndex));
	return m_pData[nIndex];
}

template <class TYPE, class ARG_TYPE>
const CArrayEx<TYPE, ARG_TYPE>& CArrayEx<TYPE, ARG_TYPE>::operator=(const CArrayEx& src)
{
	Copy(src);
	return src;
}

#endif
