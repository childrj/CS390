
// WaveEditView.h : interface of the CWaveEditView class
//

#pragma once
#include <stack>
#include <string>


using namespace std;

class CWaveEditView : public CScrollView //CView (changed for step #4)
{
	friend class CWaveEditDoc;
	friend class WaveFile; 

	bool mousePressed;
	int selectionStart; // Selected sample start
	int selectionEnd; // Selected sample end
	double zoom;			//zoom level, default 1
	int count = 0;
	int scale;
public:
	//WaveFile * originalWave;
	void addCommand(string cmd, int cmdStart, int cmdEnd, WaveFile * waveSection);
protected: // create from serialization only
	CWaveEditView();
	DECLARE_DYNCREATE(CWaveEditView)
	stack<CWaveEditDoc::Command> undoStack;
	stack<CWaveEditDoc::Command>redoStack;
	WaveFile * clip;
	
	string removeCommand(string Command);
	void eraseStack();
	void undo();
	void redo();
	
// Attributes
public:
	CWaveEditDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//set scroll extent(size of image behind window)
	virtual void OnInitialUpdate();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CWaveEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditCut();
	afx_msg void OnToolsSlowdown();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnZoomX1();
	afx_msg void OnZoom2x();
	afx_msg void OnZoom3x();
	afx_msg void OnViewZoomin();
	afx_msg void OnViewZoomout();
	afx_msg void OnViewView100();
	afx_msg void OnToolsEcho();
	afx_msg void OnToolsSpeedup();
	afx_msg void OnToolsPlaybackwards();
	afx_msg void OnToolsHyperspeed();
	afx_msg void OnToolsTurtleslow();
};

#ifndef _DEBUG  // debug version in WaveEditView.cpp
inline CWaveEditDoc* CWaveEditView::GetDocument() const
   { return reinterpret_cast<CWaveEditDoc*>(m_pDocument); }
#endif

