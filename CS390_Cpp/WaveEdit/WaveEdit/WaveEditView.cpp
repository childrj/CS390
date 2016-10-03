
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
	ON_COMMAND(ID_VIEW_ZOOMIN, &CWaveEditView::OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CWaveEditView::OnViewZoomout)
	ON_COMMAND(ID_VIEW_VIEW100, &CWaveEditView::OnViewView100)
	ON_COMMAND(ID_EDIT_UNDO, &CWaveEditView::undo)
	ON_COMMAND(ID_EDIT_REDO, &CWaveEditView::redo)
	ON_COMMAND(ID_TOOLS_PLAYBACKWARDS, &CWaveEditView::OnToolsPlaybackwards)
	ON_COMMAND(ID_TOOLS_SLOWDOWN, &CWaveEditView::OnToolsSlowdown)
	ON_COMMAND(ID_TOOLS_ECHO, &CWaveEditView::OnToolsEcho)
	ON_COMMAND(ID_TOOLS_SPEEDUP, &CWaveEditView::OnToolsSpeedup)

	ON_COMMAND(ID_TOOLS_HYPERSPEED, &CWaveEditView::OnToolsHyperspeed)
	ON_COMMAND(ID_TOOLS_TURTLESLOW, &CWaveEditView::OnToolsTurtleslow)
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
	//eraseStack();
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
	//count++;
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
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
	//if (selectionStart != selectionEnd) {
	//	pDC->Rectangle(selectionStart, 0, selectionEnd, rect.Height());
	//}
	if (selectionStart == selectionEnd) {
		pDC->MoveTo(0, 0);
		pDC->FillSolidRect(selectionStart, 0, 1, rect.Height(), RGB(0, 0, 0));
	}
	//Fill the selection rectangle
	else {
		pDC->MoveTo(0, 0);
		pDC->FillSolidRect(selectionStart, 0, selectionEnd - selectionStart, rect.Height(), RGB(0, 100, 255));
	}
	
	
	//End Draw Selection/////////////////////////////////
	
	// TODO: add draw code for native data here
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL) {
		return;
	}

	//if (count == 1)this->originalWave = wave;

	// Get dimensions of the window.
	//CRect rect;
	//GetClientRect(rect);
	
	// Set color in pen and brush for wave
	/*COLORREF*/ color = RGB(0, 255, 0);
	CPen pen2(PS_SOLID, 0, color);
	pDC->SelectObject(&pen2);
	CBrush brush2(color);
	pDC->SelectObject(&brush2);

	// Draw the wave
	pDC->MoveTo(0, 0);
	unsigned int x;
	//for (x = 0; x < rect.Width(); x++) {
	scale = wave->lastSample / rect.Width();//1000 samples / 1000 width = scale of1
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
	WaveFile * wave = pDoc->wave;
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

	//add properly formattedinfo inside struct to undo stack
	addCommand("cut", selectionStart, selectionEnd, wave);
	// Substitute old wave with new one
	w2->updateHeader();
	pDoc->wave = w2;

	//clear the variables
	this->selectionStart = 0;
	this->selectionEnd = 0;
	//update headers of wave and clipboard
	wave->updateHeader();
	pApp->clipboard->updateHeader();
	// Update window
	this->RedrawWindow();



}

void CWaveEditView::OnEditCopy()
{
	//get a pointer to doc and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//pointer to document wave file and fail check
	WaveFile * wave = pDoc->wave;
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
	addCommand("copy", selectionStart, selectionEnd, wave);

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
	WaveFile * wave = pDoc->wave;
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

	//add properly formatted to undo stack
	addCommand("paste", selectionStart, selectionEnd, wave);
	// Substitute old wave with new one and updat header
	w2->updateHeader();
	pDoc->wave = w2;
	(pDoc->wave)->updateHeader();
	//reset start/end variables

	//update the wave graph
	this->RedrawWindow();
	this->selectionStart = 0;
	this->selectionEnd = 0;
}

void CWaveEditView::OnViewZoomin()
{
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;

	//add properly formatted to undo stack
	addCommand("zoomin", 0, 0, wave);

	this->zoom = this->zoom * 2;
	RedrawWindow();
}

void CWaveEditView::OnViewZoomout()
{
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	//add properly formatted to undo stack
	addCommand("zoomout", 0, 0, wave);

	this->zoom = this->zoom / 2;
	RedrawWindow();
}

void CWaveEditView::OnViewView100()
{
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	//add properly formatted to undo stack
	addCommand("zoom100", 0, 0, wave);

	this->zoom = 1;
	RedrawWindow();
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

void CWaveEditView::addCommand(string command, int commandStart, int commandEnd, WaveFile * commandWaveSection) {
	//fill the struct
	CWaveEditDoc::Command cmd;
	cmd.cmd = command;
	cmd.cmdStart = commandStart;
	cmd.cmdEnd = commandEnd;
	cmd.waveSection = commandWaveSection;
	//push it on to stack
	undoStack.push(cmd);
}

void CWaveEditView::undo() {
	//if empty
	if (undoStack.empty())return;
	//temp stack for swap
	stack<CWaveEditDoc::Command>temp;
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//get rid of action to be undone
	zoom = 1;
	//first check if there is only one action
	if (undoStack.size() == 1) {
		CWaveEditDoc::Command popCmd = undoStack.top();
		//get info
		WaveFile * waveStart = popCmd.waveSection;
		redoStack.push(undoStack.top());
		undoStack.pop();
		//Make a new copy of the file as it originally was
		pDoc->wave = waveStart;
		pDoc->wave->updateHeader();
		this->RedrawWindow();
	}
	else //more than one
	{
		//drop action to be undone
		redoStack.push(undoStack.top());
		undoStack.pop();
		//empty undo stack into temp with reversed order
		while (!undoStack.empty()) {
			temp.push(undoStack.top());
			undoStack.pop();
		}
		//do first action here to redraw wave
		//get top struct
		CWaveEditDoc::Command popCmd = temp.top();
		//get info
		string cmd = popCmd.cmd;
		int start = popCmd.cmdStart;
		int end = popCmd.cmdEnd;
		WaveFile * wavePiece = popCmd.waveSection;
		temp.pop();
		//Make a new copy of the file as it originally was
		pDoc->wave = wavePiece;
		pDoc->wave->updateHeader();
		/////////////////////////////////////////////////
		//undo is now empty, temp is now ready to be executed
		while (!temp.empty()) {
			//get top struct
			CWaveEditDoc::Command popCmd = temp.top();
			//get info
			string cmd = popCmd.cmd;
			int start = popCmd.cmdStart;
			int end = popCmd.cmdEnd;
			WaveFile * wavePiece = popCmd.waveSection;
			//remove from temp stack
			temp.pop();
			undoStack.push(popCmd);//put it back on undo stack
			if (cmd == "cut") {
				 //parse out selection start/end
				this->selectionStart = start;
				this->selectionEnd = end;
				//do the action
				CWaveEditView::OnEditCut();
			}
			else if (cmd == "copy") {
				//parse out selection start/end
				this->selectionStart = start;
				this->selectionEnd = end;
				//do the action
				CWaveEditView::OnEditCopy();
			}
			else if (cmd == "paste") {
				this->selectionStart = start;
				this->selectionEnd = end;
				//do the action
				CWaveEditView::OnEditPaste();
			}
			else if (cmd == "zoomin") {
				CWaveEditView::OnViewZoomin();
			}
			else if (cmd == "zoomout") {
				CWaveEditView::OnViewZoomout();
			}
			else if (cmd == "zoom100" ) {
				CWaveEditView::OnViewView100();
			}
			else if (cmd == "slowdown" ) {
				WaveFile *slowWave = (pDoc->wave)->multiply_freq(0.5, 0);
				//slowWave->play();
				//modify current wave file
				pDoc->wave = slowWave;
			}
			else if (cmd == "slowdownPart") {
				WaveFile *slowWave = (pDoc->wave)->multiply_freq(0.5, selectionStart, selectionEnd);
				//slowWave->play();
				//modify current wave file
				pDoc->wave = slowWave;
			}
			else if (cmd == "speedup") {
				WaveFile * speedWave = (pDoc->wave)->multiply_freq(2, (double)start, end - start);
				//speedWave->play();
				//modify current wave file
				pDoc->wave = &speedWave[0];
			}
			else if (cmd == "echo") {
				//make new wav with echo
				WaveFile * echoWave = (pDoc->wave)->echo(0.5, 500);
				//play it
				//echoWave->play();
				//modify current wave file
				pDoc->wave = &echoWave[0];
			}
			else if (cmd == "playBackwards") {
				//parse out selection start/end
				this->selectionStart = start;
				this->selectionEnd = end;
				//do the action
				CWaveEditView::OnToolsPlaybackwards();
			}
			else {
				throw invalid_argument("received invalid string");
			}

			
		}//while
		this->RedrawWindow();

	}//else
}

void CWaveEditView::redo() {
		int x = 0;
	if (redoStack.empty())return;
	//do all this again until figure out how to use same object across classes
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//put existing wave file into wave and fail check
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	
	//move redo action to undo stack;
	undoStack.push(redoStack.top());
	//get rid of redo action
	CWaveEditDoc::Command cmd = redoStack.top();
	redoStack.pop();
	if (cmd.cmd == "cut") {
		//parse out selection start/end
		int start = cmd.cmdStart;
		int end = cmd.cmdEnd;
		this->selectionStart = start;
		this->selectionEnd = end;
		//do the action
		CWaveEditView::OnEditCut();
	}
	else if (cmd.cmd == "copy") {
		//parse out selection start/end
		int start = cmd.cmdStart;
		int end = cmd.cmdEnd;
		this->selectionStart = start;
		this->selectionEnd = end;
		//do the action
		CWaveEditView::OnEditCopy();
	}
	else if (cmd.cmd == "paste") {
		//parse out selection start/end
		int start = cmd.cmdStart;
		int end = cmd.cmdEnd;
		this->selectionStart = start;
		this->selectionEnd = end;
		//do the action
		CWaveEditView::OnEditPaste();
	}
	else if (cmd.cmd == "zoomin") {
		CWaveEditView::OnViewZoomin();
	}
	else if (cmd.cmd == "zoomout" ) {
		CWaveEditView::OnViewZoomout();
	}
	else if (cmd.cmd == "zoom100") {
		CWaveEditView::OnViewView100();
	}
	else if (cmd.cmd == "slowdown") {
		WaveFile *slowWave = wave->multiply_freq(0.5, 0);
		slowWave->play();
		//modify current wave file
		*wave = slowWave[0];
	}
	else if (cmd.cmd == "speedup") {
		WaveFile * speedWave = wave->multiply_freq(2, 0);
		speedWave->play();
		//modify current wave file
		*wave = speedWave[0];
	}
	else if (cmd.cmd == "echo") {
		//make new wav with echo
		WaveFile * echoWave = wave->echo(0.5, 500);
		//play it
		echoWave->play();
		//modify current wave file
		*wave = echoWave[0];
	}
	RedrawWindow();
}

void CWaveEditView::OnToolsSlowdown()
{
	// TODO: Add your command handler code here
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile *slowWave;
	int test = 0;
	CSize size = GetTotalSize();
	if (selectionEnd !=  selectionStart) { 
		int start = wave->lastSample * (double)this->selectionStart / (double)size.cx;
		int end = wave->lastSample * (double)this->selectionEnd / (double)size.cx;
		addCommand("slowdownPart", start, end, wave);
		slowWave = wave->multiply_freq(0.5, start, end); 
		test = 1;
	}
	else {
		addCommand("slowdown", 0, 0, wave);
		slowWave = wave->multiply_freq(0.5, 0);
		test = 2;
	}
	//slowWave->play();
	//modify current wave file
	slowWave->updateHeader();
	pDoc->wave = slowWave;
	this->RedrawWindow();
	this->selectionStart = 0;
	this->selectionEnd = 0;
	//added from tutorial
	//datanew[2i] = data[i]
	//datanew[21+1] = data[i];

}

void CWaveEditView::OnToolsPlaybackwards()
{
	// TODO: Add your command handler code here
		CWaveEditDoc* pDoc = GetDocument();

		ASSERT_VALID(pDoc);
		if (!pDoc)return;

		WaveFile * w2 = pDoc->wave;
		if (w2->hdr == NULL)return;
		//add properly formattedinfo inside struct to undo stack
		addCommand("playBackwards", selectionStart, selectionEnd, w2);
		// Get dimensions of the window.
		CRect rect;
		GetClientRect(rect);
		int width = (w2->maxSamples / w2->sampleRate) * scale; 
		double startms = (1000.0 * w2->lastSample / w2->sampleRate) * this->selectionStart / width;
		double endms = (1000.0 * w2->lastSample / w2->sampleRate) * this->selectionEnd / width;

		if (startms == endms) {
			w2 = w2->reverse();
			w2->updateHeader();
			pDoc->wave = w2;
		}
		else {
			WaveFile * edit = new WaveFile(w2->numChannels, w2->sampleRate, w2->bitsPerSample);
			edit = w2->get_fragment(startms, endms);
			edit->updateHeader();
			edit = edit->reverse();
			edit->updateHeader();

			w2 = w2->replace_fragment(startms, endms, edit);
			w2->updateHeader();
			pDoc->wave = w2;
		}

		this->selectionStart = 0;
		this->selectionEnd = 0;

		this->RedrawWindow();


	

}

void CWaveEditView::OnToolsEcho()
{
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	//make new wav with echo
	WaveFile * echoWave = wave->echo(0.5, 500);
	//play it
	//echoWave->play();
	addCommand("echo", 0, 0, wave);
	//modify current wave file
	pDoc->wave = &echoWave[0];
	(pDoc->wave)->updateHeader();
	//added from tutorial
	//datanew[i] = data[i] + atenuation * data[i - delay];

}

void CWaveEditView::OnToolsSpeedup()
{/*
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile * speedWave = wave->multiply_freq(2, 0);
	addCommand("speedup", 0, 0, wave);
	//speedWave->play();
	//modify current wave file
	pDoc->wave = &speedWave[0];
	(pDoc->wave)->updateHeader();
	//added from tutorial
	//datanew[i] = data[s*i];
	*/
	// TODO: Add your command handler code here
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile *slowWave;
	int test = 0;
	CSize size = GetTotalSize();
	if (selectionEnd != selectionStart) {
		int start = wave->lastSample * (double)this->selectionStart / (double)size.cx;
		int end = wave->lastSample * (double)this->selectionEnd / (double)size.cx;
		addCommand("speedupPart", start, end, wave);
		slowWave = wave->multiply_freq(2, start, end);
		test = 1;
	}
	else {
		addCommand("speedup", 0, 0, wave);
		slowWave = wave->multiply_freq(2, 0);
		test = 2;
	}
	//slowWave->play();
	//modify current wave file
	slowWave->updateHeader();
	pDoc->wave = slowWave;
	this->RedrawWindow();
	this->selectionStart = 0;
	this->selectionEnd = 0;
	//added from tutorial
	//datanew[2i] = data[i]
	//datanew[21+1] = data[i];

}

void CWaveEditView::OnToolsHyperspeed()
{/*
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile * speedWave = wave->multiply_freq(2, 0);
	addCommand("speedup", 0, 0, wave);
	//speedWave->play();
	//modify current wave file
	pDoc->wave = &speedWave[0];
	(pDoc->wave)->updateHeader();
	//added from tutorial
	//datanew[i] = data[s*i];
	*/
		// TODO: Add your command handler code here
		CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile *slowWave;
	int test = 0;
	CSize size = GetTotalSize();
	if (selectionEnd != selectionStart) {
		int start = wave->lastSample * (double)this->selectionStart / (double)size.cx;
		int end = wave->lastSample * (double)this->selectionEnd / (double)size.cx;
		addCommand("speedupPart", start, end, wave);
		slowWave = wave->multiply_freq(8, start, end);
		test = 1;
	}
	else {
		addCommand("speedup", 0, 0, wave);
		slowWave = wave->multiply_freq(8, 0);
		test = 2;
	}
	//slowWave->play();
	//modify current wave file
	slowWave->updateHeader();
	pDoc->wave = slowWave;
	this->RedrawWindow();
	this->selectionStart = 0;
	this->selectionEnd = 0;
	//added from tutorial
	//datanew[2i] = data[i]
	//datanew[21+1] = data[i];

}


void CWaveEditView::OnToolsTurtleslow()
{/*
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	//get document(the file) and fail check
	CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile * speedWave = wave->multiply_freq(2, 0);
	addCommand("speedup", 0, 0, wave);
	//speedWave->play();
	//modify current wave file
	pDoc->wave = &speedWave[0];
	(pDoc->wave)->updateHeader();
	//added from tutorial
	//datanew[i] = data[s*i];
	*/
		// TODO: Add your command handler code here
		CWaveEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)return;
	//make a new wave pointer and set it to the wave just loaded then failcheck
	WaveFile * wave = pDoc->wave;
	if (wave->hdr == NULL)return;
	WaveFile *slowWave;
	int test = 0;
	CSize size = GetTotalSize();
	if (selectionEnd != selectionStart) {
		int start = wave->lastSample * (double)this->selectionStart / (double)size.cx;
		int end = wave->lastSample * (double)this->selectionEnd / (double)size.cx;
		addCommand("speedupPart", start, end, wave);
		slowWave = wave->multiply_freq(0.1, start, end);
		test = 1;
	}
	else {
		addCommand("speedup", 0, 0, wave);
		slowWave = wave->multiply_freq(0.1, 0);
		test = 2;
	}
	//slowWave->play();
	//modify current wave file
	slowWave->updateHeader();
	pDoc->wave = slowWave;
	this->RedrawWindow();
	this->selectionStart = 0;
	this->selectionEnd = 0;
	//added from tutorial
	//datanew[2i] = data[i]
	//datanew[21+1] = data[i];

}
