
// WaveEditView.cpp : implementation of the CWaveEditView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WaveEdit.h"
#endif

#include "WaveEditDoc.h"
#include "WaveEditView.h"

#include "iostream"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWaveEditView

IMPLEMENT_DYNCREATE(CWaveEditView, CScrollView)

BEGIN_MESSAGE_MAP(CWaveEditView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_CUT, &CWaveEditView::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CWaveEditView::OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, &CWaveEditView::OnEditCopy)
	ON_COMMAND(ID_ZOOM_X1, &CWaveEditView::OnZoomX1)
	ON_COMMAND(ID_ZOOM_2X, &CWaveEditView::OnZoom2x)
	ON_COMMAND(ID_ZOOM_3X, &CWaveEditView::OnZoom3x)
	ON_COMMAND(ID_VIEW_ZOOMIN, &CWaveEditView::OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CWaveEditView::OnViewZoomout)
	ON_COMMAND(ID_VIEW_VIEW100, &CWaveEditView::OnViewView100)
	ON_COMMAND(ID_EDIT_UNDO, &CWaveEditView::undo)
	ON_COMMAND(ID_EDIT_REDO, &CWaveEditView::redo)

END_MESSAGE_MAP()

// CWaveEditView construction/destruction

CWaveEditView::CWaveEditView()
{
	// TODO: add construction code here
	mousePressed = false;
	selectionStart = 0;
	selectionEnd = 0;
	zoom = 1;

}

CWaveEditView::~CWaveEditView()
{
	eraseStack();

}

BOOL CWaveEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	
	return CView::PreCreateWindow(cs);
}

// CWaveEditView drawing

void CWaveEditView::OnDraw(CDC* pDC)
{
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//Draw Selection///////////////////////////////
	
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	
	// Set color in pen and brush for selection
	COLORREF color = RGB(0, 200, 200);
	CPen pen1(PS_SOLID, 0, color);
	pDC->SelectObject(&pen1);
	CBrush brush1(color);
	pDC->SelectObject(&brush1);
	// Draw selection if any
	if (selectionStart != selectionEnd) {
		pDC->Rectangle(selectionStart, 0, selectionEnd, rect.Height());
	}
	//End Draw Selection/////////////////////////////////
	
	// TODO: add draw code for native data here
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL) {
		return;
	}
	// Get dimensions of the window.
	//CRect rect;
	//GetClientRect(rect);
	
	// Set color in pen and brush for wave
	/*COLORREF*/ color = RGB(0, 255, 0);
	CPen pen2(PS_SOLID, 0, color);
	pDC->SelectObject(&pen2);
	CBrush brush2(color);
	pDC->SelectObject(&brush2);

	/*
	if (selectionStart == selectionEnd){
		pDC->MoveTo(0, 0);
		pDC->FillSolidRect(selectionStart, 0, 1, rect.Height(), RGB(0, 0, 0));
	}
	else {
		pDC->MoveTo(0, 0);
		pDC->FillSolidRect(selectionStart, 0, selectionEnd-selectionStart, rect.Height(), RGB(0, 100, 255));
	}
	*/
	
	// Draw the wave
	pDC->MoveTo(0, 0);
	unsigned int x;
	//for (x = 0; x < rect.Width(); x++) {
	int scale = wave->lastSample / rect.Width();//1000 samples / 1000 width = scale of1
	//get scale of samples vs rect width		//1000 samples / 10000 width = scale of 1/10
	int wavZoom = (zoom * wave->lastSample) / scale;
	//z*w->ls gives a larger sample count, divide by scale to normalize
	for (x = 0; x < wavZoom; x++ ){
		// Assuming the whole file will be fit in the window, for every x value in the window
		// we need to find the equivalent sample in the wave file.
		//float val = wave->get_sample((int)(x*wave->lastSample / rect.Width()));
		float val = wave->get_sample((int)x * scale / this->zoom);
		// We need to fit the sound also in the y axis. The y axis goes from 0 in the
		// top of the window to rect.Height at the bottom. The sound goes from -32768 to 32767
		// we scale it in that way.
		int y = (int)((val + 32768) * (rect.Height() - 1) / (32767 + 32768));
		pDC->LineTo(x, rect.Height() - y);
	}
	
	CSize sizeTotal;
	sizeTotal.cx = x;
	//sizeTotal.cy = 10000;
	SetScrollSizes(MM_TEXT, sizeTotal);

}


void CWaveEditView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	//initial scroll sizes
	CSize sizeTotal;
	//sizeTotal.cx = 10000;
	//sizeTotal.cy = 10000;
	SetScrollSizes(MM_TEXT, sizeTotal);

}

// CWaveEditView printing

BOOL CWaveEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWaveEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWaveEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CWaveEditView diagnostics

#ifdef _DEBUG
void CWaveEditView::AssertValid() const
{
	CView::AssertValid();
}

void CWaveEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWaveEditDoc* CWaveEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWaveEditDoc)));
	return (CWaveEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CWaveEditView message handlers


void CWaveEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mousePressed = true;
	selectionStart = point.x;
	selectionEnd = point.x;
	
	CScrollView::OnLButtonDown(nFlags, point);
}


void CWaveEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mousePressed = false;
	this->selectionEnd = point.x;
	if (selectionEnd < selectionStart) {
		int temp = selectionEnd;
		selectionEnd = selectionStart;
		selectionStart = temp;
	}
	this->RedrawWindow();
	CScrollView::OnLButtonUp(nFlags, point);
}


void CWaveEditView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnMouseMove(nFlags, point);

	if (mousePressed) {
		selectionEnd = point.x;
		RedrawWindow();
	}
}


void CWaveEditView::OnEditCut()
{
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL)return;
	//dynamic cast pApp as persistent clipboard, then clear it out with new, then fail check
	CWaveEditApp* pApp = dynamic_cast<CWaveEditApp*>(AfxGetApp());
	pApp->clipboard = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
	if (pApp->clipboard == NULL)return;
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	int rectWidth = (wave->maxSamples / wave->sampleRate) * 500; //rect.Width();
	// Scale the start and end of the selection.
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rectWidth;
	// Scale the start and end of the selection.
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rectWidth;
	// Copy first the fragment to the clipboard
	pApp->clipboard = wave->get_fragment(startms, endms);
	// Remove the fragment
	WaveFile * w2 = wave->remove_fragment(startms, endms);
	// Substitute old wave with new one
	pDoc->wave = *w2;
	//clear the variables

	//update headers of wave and clipboard
	wave->updateHeader();
	pApp->clipboard->updateHeader();
	// Update window
	this->RedrawWindow();
	//add properly formatted to undo stack
	//action(period)start(period)end
	addCommand("cut." + to_string(selectionStart) + "." + to_string(selectionEnd));

	this->selectionStart = 0;
	this->selectionEnd = 0;
}

void CWaveEditView::OnEditCopy()
{
	//get a pointer to doc and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//pointer to document wave file and fail check
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL)return;
	//make a persistent clipboard pointer and clear it as new
	CWaveEditApp* pApp = dynamic_cast<CWaveEditApp*>(AfxGetApp());
	pApp->clipboard = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	int rectWidth = (wave->maxSamples / wave->sampleRate) * 500; //rect.Width();
	//scale the start and end of the selection
	double startms = (1000.0 * wave->lastSample / wave->sampleRate ) * this->selectionStart / rectWidth;
	double endms = (1000.0 * wave->lastSample / wave->sampleRate ) * this->selectionEnd / rectWidth;
	//put the selection in the clipboard
	pApp->clipboard = wave->get_fragment(startms, endms);
	//update header of clipboard...wave was not changed
	pApp->clipboard->updateHeader();
	//reset variables

	//add properly formatted to undo stack
	addCommand("copy." + to_string(selectionStart) + "." + to_string(selectionEnd));

	this->selectionStart = 0;
	this->selectionEnd = 0;
}

void CWaveEditView::OnEditPaste()
{
	//get document and make sure it worked
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//put existing wave file into wave and fail check
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL)return;
	//dynamic cast pApp as clipboard, DON'T make it new, then fail check
	CWaveEditApp* pApp = dynamic_cast<CWaveEditApp*>(AfxGetApp());
	//pApp->clipboard = new WaveFile(wave->numChannels, wave->sampleRate, wave->bitsPerSample);
	if (pApp->clipboard == NULL)return;
	// Get dimensions of the window.
	CRect rect;
	GetClientRect(rect);
	int rectWidth = (wave->maxSamples / wave->sampleRate) * 500; //rect.Width();
	// Scale the start of the selection.
	double startms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionStart / rectWidth;
	double endms = (1000.0 * wave->lastSample / wave->sampleRate) * this->selectionEnd / rectWidth;
	//remove whatever section is highlighted as being pasted over
	wave->remove_fragment(startms, endms);
	// insert the fragment from clipboard at start of selection
	WaveFile * w2 = wave->insert_fragment(startms, pApp->clipboard);
	// Substitute old wave with new one and updat header
	pDoc->wave = *w2;
	wave->updateHeader();
	//reset start/end variables

	//update the wave graph
	this->RedrawWindow();
	//add properly formatted to undo stack
	addCommand("paste." + to_string(selectionStart) + "." + to_string(selectionEnd));

	this->selectionStart = 0;
	this->selectionEnd = 0;
}





void CWaveEditView::OnZoomX1()
{
	// TODO: Add your command handler code here
	this->zoom = 1;
	RedrawWindow();

}


void CWaveEditView::OnZoom2x()
{
	// TODO: Add your command handler code here
	this->zoom = 2;
	RedrawWindow();
}


void CWaveEditView::OnZoom3x()
{
	// TODO: Add your command handler code here
	this->zoom = 3;
	RedrawWindow();
}


void CWaveEditView::OnViewZoomin()
{
	// TODO: Add your command handler code here
	this->zoom = this->zoom * 2;
	RedrawWindow();

	//add properly formatted to undo stack
	undoStack.push("zoomin");
}


void CWaveEditView::OnViewZoomout()
{
	// TODO: Add your command handler code here
	this->zoom = this->zoom / 2;
	RedrawWindow();

	//add properly formatted to undo stack
	addCommand("zoomout");
}


void CWaveEditView::OnViewView100()
{
	// TODO: Add your command handler code here
	this->zoom = 1;
	RedrawWindow();

	//add properly formatted to undo stack
	addCommand("zoom100");
}

//UNDO REDO Section
void CWaveEditView::eraseStack()
{
	while (!undoStack.empty()) {
		delete &undoStack.top();
		undoStack.pop();
	}
	while (!redoStack.empty()) {
		delete &redoStack.top();
		redoStack.pop();
	}

}

void CWaveEditView::addCommand(string Command) {
	undoStack.push(Command);
}


void CWaveEditView::undo() {
	if (undoStack.empty())return;
	stack<string>temp;
	
	//do all this again until figure out how to use same object across classes
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//put existing wave file into wave and fail check
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL)return;
	
	//move undo action to redo stack;
	redoStack.push(undoStack.top());
	//get rid of undo action
	//undoStack.pop();
	//empty undo stack into temp with reversed order
	while (!undoStack.empty()) {
		temp.push(undoStack.top());
		undoStack.pop();
	}
	//undo is now empty, temp is now ready to be executed
	while (!temp.empty()) {
		string s = temp.top();//put string in s
		temp.pop();//remove from temp stack
		undoStack.push(s);//put it back on undo stack
		if (s.find("cut")==0) {
			 //parse out selection start/end
			string test = s;
			s = s.substr(4, s.length());
			int i = s.find(".");
			string start = s.substr(0, i);
			s = s.substr(i+1, s.length());
			string end = s;
			this->selectionStart = stoi(start);
			this->selectionEnd = stoi(end);
			//do the action
			CWaveEditView::OnEditCut();
		}
		else if (s.find("copy") == 0) {
			//parse out selection start/end
			string start = s.substr(4, s.find("."));
			string end = s.substr(s.find("."+start + "."), s.length());
			this->selectionStart = stoi(start);
			this->selectionEnd = stoi(end);
			//do the action
			CWaveEditView::OnEditCopy();
		}
		else if (s.find("paste") == 0) {
			//parse out selection start/end
			string start = s.substr(5, s.find("."));
			string end = s.substr(s.find("." + start + "."), s.length());
			this->selectionStart = stoi(start);
			this->selectionEnd = stoi(end);
			//do the action
			CWaveEditView::OnEditPaste();
		}
		else if (s.find("zoomin") == 0) {
			CWaveEditView::OnViewZoomin();
		}
		else if (s.find("zoomout") == 0) {
			CWaveEditView::OnViewZoomout();
		}
		else if (s.find("zoom100") == 0) {
			CWaveEditView::OnViewView100();
		}
		else if (s.find("slowdown") == 0) {
			WaveFile *slowWave = wave->multiply_freq(0.5, 0);
			slowWave->play();
			//modify current wave file
			*wave = slowWave[0];
		}
		else if (s.find("speedup") == 0) {
			WaveFile * speedWave = wave->multiply_freq(2, 0);
			speedWave->play();
			//modify current wave file
			*wave = speedWave[0];
		}
		else if (s.find("echo") == 0) {
			//make new wav with echo
			WaveFile * echoWave = wave->echo(0.5, 500);
			//play it
			echoWave->play();
			//modify current wave file
			*wave = echoWave[0];
		}
		else {
			throw invalid_argument("received invalid string");
		}


	}
	RedrawWindow();

}

void CWaveEditView::redo() {
	if (redoStack.empty())return;
	//do all this again until figure out how to use same object across classes
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//put existing wave file into wave and fail check
	WaveFile * wave = &pDoc->wave;
	if (wave->hdr == NULL)return;
	
	//move redo action to undo stack;
	undoStack.push(redoStack.top());
	//get rid of redo action
	string s = redoStack.top();
	redoStack.pop();
	if (s.find("cut") == 0) {
		//parse out selection start/end
		string start = s.substr(3, s.find("."));
		string end = s.substr(s.find("." + start + "."), s.length());
		this->selectionStart = stoi(start);
		this->selectionEnd = stoi(end);
		//do the action
		CWaveEditView::OnEditCut();
	}
	else if (s.find("copy") == 0) {
		//parse out selection start/end
		string start = s.substr(4, s.find("."));
		string end = s.substr(s.find("." + start + "."), s.length());
		this->selectionStart = stoi(start);
		this->selectionEnd = stoi(end);
		//do the action
		CWaveEditView::OnEditCopy();
	}
	else if (s.find("paste") == 0) {
		//parse out selection start/end
		string start = s.substr(5, s.find("."));
		string end = s.substr(s.find("." + start + "."), s.length());
		this->selectionStart = stoi(start);
		this->selectionEnd = stoi(end);
		//do the action
		CWaveEditView::OnEditPaste();
	}
	else if (s.find("zoomin") == 0) {
		CWaveEditView::OnViewZoomin();
	}
	else if (s.find("zoomout") == 0) {
		CWaveEditView::OnViewZoomout();
	}
	else if (s.find("zoom100") == 0) {
		CWaveEditView::OnViewView100();
	}
	else if (s.find("slowdown") == 0) {
		WaveFile *slowWave = wave->multiply_freq(0.5, 0);
		slowWave->play();
		//modify current wave file
		*wave = slowWave[0];
	}
	else if (s.find("speedup") == 0) {
		WaveFile * speedWave = wave->multiply_freq(2, 0);
		speedWave->play();
		//modify current wave file
		*wave = speedWave[0];
	}
	else if (s.find("echo") == 0) {
		//make new wav with echo
		WaveFile * echoWave = wave->echo(0.5, 500);
		//play it
		echoWave->play();
		//modify current wave file
		*wave = echoWave[0];
	}
	RedrawWindow();
}