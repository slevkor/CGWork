// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"
#include "Matrix.h"


// For Status Bar access
#include "MainFrm.h"

// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//hw2
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_SPACE_WORLD, OnSpaceView)
	ON_UPDATE_COMMAND_UI(ID_SPACE_WORLD, OnUpdateSpaceView)
	ON_COMMAND(ID_SPACE_OBECT, OnSpaceObject)
	ON_UPDATE_COMMAND_UI(ID_SPACE_OBECT, OnUpdateSpaceObject)
	//
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;	
	m_bIsPerspective = false;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled=true;
}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	//hw2
	SensitiveRotate = 0.2;
	SensitiveTranslate=1;
	SensitiveScale=1;
	spaceCoridantes = View;
	axis = X;
	backgroundColor = COLORREF(RGB(255, 255, 255));
	linesColor = COLORREF(RGB(0, 0, 0));
	boundingBoxColor = COLORREF(RGB(255, 0, 0));
	AllPolys = NULL;
	stamX = 0;
	stamY = 0;
	degreesRotatedOnX = 0;
	degreesRotatedOnY = 0;
	degreesRotatedOnZ = 0;
	movedOnX = 0;
	movedOnY = 0;
	movedOnZ = 0;
	rotationMatrixX;
	rotationMatrixY;
	rotationMatrixZ;
	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);

	if ( NULL == m_pDC ) { // failure to get DC
		AfxMessageBox(_T("Couldn't get a valid DC."));
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}





BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	// return CView::OnEraseBkgnd(pDC);
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	    return;

	//hw2
	// paint the whole screen with backgroundColor
	CRect rect;
	rect.bottom = m_WindowHeight;
	rect.left = 0;
	rect.right = m_WindowWidth;
	rect.top = 0;
	pDC->FillSolidRect(rect, backgroundColor);

	//hw2 - the drawing
	IPPolygonStruct* polys = AllPolys;
	while (polys != NULL){
		IPVertexStruct* vertices = polys->PVertex;
		while (vertices != NULL){
			if (vertices->Pnext == NULL){
				break;
			}
			//int x1 = (vertices->Coord[0]) * m_AspectRatio * 100 + (m_WindowWidth / 2);
			//int y1 = (vertices->Coord[1]) * -m_AspectRatio * 100 + (m_WindowHeight / 2);
			//int x2 = (vertices->Pnext->Coord[0]) * m_AspectRatio * 100 + (m_WindowWidth / 2);
			//int y2 = (vertices->Pnext->Coord[1]) * -m_AspectRatio * 100 + (m_WindowHeight / 2);
			
			//hw2main loop?
			matrix newXYZ1(1, MATRIX_SIZE);
			newXYZ1.setXYZ(vertices->Coord);
			matrix newXYZ2(1, MATRIX_SIZE);
			newXYZ2.setXYZ(vertices->Pnext->Coord);
			double x1, y1, x2, y2;
			newXYZ1.selfRotation(degreesRotatedOnX, X);
			newXYZ1.selfRotation(degreesRotatedOnY, Y);
			newXYZ1.selfRotation(degreesRotatedOnZ, Z);
			newXYZ2.selfRotation(degreesRotatedOnX, X);
			newXYZ2.selfRotation(degreesRotatedOnY, Y);
			newXYZ2.selfRotation(degreesRotatedOnZ, Z);
			newXYZ1.selfTranslate(movedOnX, movedOnY, movedOnZ);
			newXYZ2.selfTranslate(movedOnX, movedOnY, movedOnZ);
			if (m_nView == ID_VIEW_ORTHOGRAPHIC){
				x1 = newXYZ1.getX();
				y1 = newXYZ1.getY();
				x2 = newXYZ2.getX();
				y2 = newXYZ2.getY();
				x1 = x1 * m_AspectRatio * 100 + (m_WindowWidth / 2);
				x2 = x2 * m_AspectRatio * 100 + (m_WindowWidth / 2);
				y1 = y1 * -m_AspectRatio * 100 + (m_WindowHeight / 2);
				y2 = y2 * -m_AspectRatio * 100 + (m_WindowHeight / 2);
				drawLine(pDC, linesColor, x1, y1, x2, y2);
			}
			
			
			vertices = vertices->Pnext;
		}
		polys = polys->Pnext;
	}

	/*
	for (int w = 0; w < m_WindowWidth; w++)
	{
		drawLine(pDC, linesColor, m_WindowWidth / 2, m_WindowHeight / 2, w, 0+50);
	}
	for (int h = 0; h < m_WindowHeight; h++)
	{
		drawLine(pDC, linesColor, m_WindowWidth / 2, m_WindowHeight / 2, m_WindowWidth-50, h);
	}
	for (int w = 0; w < m_WindowWidth; w++)
	{
		drawLine(pDC, linesColor, m_WindowWidth / 2, m_WindowHeight / 2, m_WindowWidth - w, m_WindowHeight-50);
	}
	for (int h = 0; h < m_WindowHeight; h++)
	{
		drawLine(pDC, linesColor, m_WindowWidth / 2, m_WindowHeight / 2, 0+50, m_WindowHeight - h);
	}*/
	
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy() 
{
	CView::OnDestroy();

	// delete the DC
	if ( m_pDC ) {
		delete m_pDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad() 
{
	TCHAR szFilters[] = _T ("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(true, (CString)"itd", (CString)"*.itd", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal () == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);
		
		AllPolys = getAllPolys();
		getMinMaxPoints(AllPolys);
		xBiggestDistance = maxPoints[0] - minPoints[0];
		yBiggestDistance = maxPoints[1] - minPoints[1];
		zBiggestDistance = maxPoints[2] - minPoints[2];
		m_nAction = ID_ACTION_ROTATE;
		Invalidate();	// force a WM_PAINT for drawing.
	} 

}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}




// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	axis = X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(axis == X);
}


void CCGWorkView::OnAxisY() 
{
	axis = Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(axis == Y);
}


void CCGWorkView::OnAxisZ() 
{
	axis = Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(axis == Z);
}


// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat() 
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud() 
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants() 
{
	CLightDialog dlg;

	for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	{	    
	    dlg.SetDialogData((LightID)id,m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT,m_ambientLight);

	if (dlg.DoModal() == IDOK) 
	{
	    for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	    {
		m_lights[id] = dlg.GetDialogData((LightID)id);
	    }
	    m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}	
	Invalidate();
}

// SPACE HANDLERS

void CCGWorkView::OnUpdateSpaceView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(spaceCoridantes == View);
}
void CCGWorkView::OnSpaceView()
{
	spaceCoridantes = View;
}
void CCGWorkView::OnUpdateSpaceObject(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(spaceCoridantes == Object);
}
void CCGWorkView::OnSpaceObject()
{
	spaceCoridantes = Object;
}


// mouse methods
void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point)
{
	CString labelStr;
	CString helpStr;
	CString finalStr;
	CMainFrame* pMF = (CMainFrame*)GetParentFrame();
	double degrees;
	
	if (AllPolys != NULL){
		if (GetCapture() == this)
		{

			if (point.x > stamX){
				//moving right
				mouseMovement = 1;

				if (m_nAction == ID_ACTION_ROTATE){
					if (axis == X){
						degreesRotatedOnX += SensitiveRotate;
					}
					if (axis == Y){
						degreesRotatedOnY += SensitiveRotate;
					}
					if (axis == Z){
						degreesRotatedOnZ += SensitiveRotate;
					}
				}
				if (m_nAction == ID_ACTION_TRANSLATE){
					if (axis == X){
						movedOnX++;
					}
					if (axis == Y){
						movedOnY++;
					}
					if (axis == Z){
						movedOnZ++;
					}
				}
			}
			else {
				//moving left
				mouseMovement = -1;

				if (m_nAction == ID_ACTION_ROTATE){
					if (axis == X){
						degreesRotatedOnX -= SensitiveRotate;
					}
					if (axis == Y){
						degreesRotatedOnY -= SensitiveRotate;
					}
					if (axis == Z){
						degreesRotatedOnZ -= SensitiveRotate;
					}
				}
				if (m_nAction == ID_ACTION_TRANSLATE){
					if (axis == X){
						movedOnX--;
					}
					if (axis == Y){
						movedOnY--;
					}
					if (axis == Z){
						movedOnZ--;
					}
				}
			}
			Invalidate();
		

			//hw2 add label to the movement
			if (m_nAction == ID_ACTION_ROTATE){
				labelStr = "Rotated by : ";
				helpStr = " degrees";
				if (axis == X){
					degrees = degreesRotatedOnX;
				}
				if (axis == Y){
					degrees = degreesRotatedOnY;
				}
				if (axis == Z){
					degrees = degreesRotatedOnZ;
				}
				finalStr.Format(_T("%s%f%s"), labelStr, degrees, helpStr);
				pMF->getStatusBar().SetPaneText(0, finalStr);
			}
			if (m_nAction == ID_ACTION_SCALE){
				labelStr = "Scaled by : ";
				helpStr = "";
			
			}
			if (m_nAction == ID_ACTION_TRANSLATE){
				labelStr = "Moved by : ";
				helpStr = "";
				if (axis == X){
					degrees = movedOnX;
				}
				if (axis == Y){
					degrees = movedOnY;
				}
				if (axis == Z){
					degrees = movedOnZ;
				}
				finalStr.Format(_T("%s%f%s"), labelStr, degrees, helpStr);
				pMF->getStatusBar().SetPaneText(0, finalStr);
			}
		}
	}
	
	stamX = point.x;

	CView::OnMouseMove(nFlags, point);
}

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();	// capture the mouse 'left button up' command
	mouseMovement = 0;
	stamX = point.x;		// keep x coordinate
	stamY = point.y;		// keep y coordinate

	CView::OnLButtonDown(nFlags, point);
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point)
{
	
	if (GetCapture() == this) ::ReleaseCapture();
	mouseMovement = 0;

	Invalidate();
	CView::OnLButtonUp(nFlags, point);
}

//helper methods
void CCGWorkView::getMinMaxPoints(IPPolygonStruct* polys)
{
	maxPoints[3] = { 0 };
	minPoints[3] = { 0 };
	IPPolygonStruct* tempPolys = polys;
	while (tempPolys != NULL){
		IPVertexStruct* vertices = tempPolys->PVertex;
		while (vertices != NULL){
			for (int i = 0; i < 2; i++)
			{
				if (vertices->Coord[i] >= maxPoints[i]){
					maxPoints[i] = vertices->Coord[i];
				}
				if (vertices->Coord[i] <= minPoints[i]){
					minPoints[i] = vertices->Coord[i];
				}
			}
			
			vertices = vertices->Pnext;
		}
		tempPolys = tempPolys->Pnext;
	}
	
}

// drawing methods
void CCGWorkView::drawBoudingBox(){

}

void CCGWorkView::drawLine(CDC* pDC, COLORREF color, int x1, int y1, int x2, int y2){
	int signX,signY,xOne, xTwo, yOne, yTwo, dx, dy, d, delta1, delta2;
	chooseAndPutPoints(&signX, &signY,&xOne, &xTwo, &yOne, &yTwo, x1, y1, x2, y2);
	dx = abs(xTwo - xOne);
	dy = abs(yTwo - yOne);
	if (dx == 0){
		//draw vertcal line
		pDC->SetPixel(xOne, yOne, color);
		int ySmall = yOne;
		int yBig = yTwo;
		if (yOne > yTwo){
			yBig = yOne;
			ySmall = yTwo;
		}
		while (ySmall < yBig){
			ySmall = ySmall + 1;
			pDC->SetPixel(xOne, ySmall, color);
		}
		return;
	}
	if (dy == 0){
		//draw horizinal line
		pDC->SetPixel(xOne, yOne, color);
		int xSmall = xOne;
		int xBig = xTwo;
		if (xOne > xTwo){
			xBig = xOne;
			xSmall = xTwo;
		}
		while (xSmall < xBig){
			xSmall = xSmall + 1;
			pDC->SetPixel(xSmall, yOne, color);
		}
		return;
	}
	if (dy / dx < 1){
		d = 2 * dy - dx;
		delta1 = 2 * dy;
		delta2 = 2 * (dy - dx);
		pDC->SetPixel(xOne, yOne, color);
		if (xTwo < xOne){
			xOne = xOne*signX;
			yOne = yOne*signY;
			xTwo = xTwo*signX;
			while (xOne < xTwo){
				if (d < 0){
					d = d + delta1;
					xOne = xOne + 1;
				}
				else{
					d = d + delta2;
					xOne = xOne + 1;
					yOne = yOne + 1;
				}
				xOne = xOne*signX;
				yOne = yOne*signY;
				pDC->SetPixel(xOne, yOne, color);
				xOne = xOne*signX;
				yOne = yOne*signY;
			}
		}
		else{
			xOne = xOne*signX;
			yOne = yOne*signY;
			while (xOne < xTwo){
				if (d < 0){
					d = d + delta1;
					xOne = xOne + 1;
				}
				else{
					d = d + delta2;
					xOne = xOne + 1;
					yOne = yOne + 1;
				}
				xOne = xOne*signX;
				yOne = yOne*signY;
				pDC->SetPixel(xOne, yOne, color);
				xOne = xOne*signX;
				yOne = yOne*signY;
			}
		}
	}
	else{
		d = 2 * dx - dy;
		delta1 = 2 * dx;
		delta2 = 2 * (dx - dy);
		pDC->SetPixel(xOne, yOne, color);
		xOne = xOne*signX;
		yOne = yOne*signY;
		yTwo = yTwo*signY;
		while (yOne < yTwo){
			if (d < 0){
				d = d + delta1;
				yOne = yOne + 1;
			}
			else{
				d = d + delta2;
				xOne = xOne + 1;
				yOne = yOne + 1;
			}
			xOne = xOne*signX;
			yOne = yOne*signY;
			pDC->SetPixel(xOne, yOne, color);
			xOne = xOne*signX;
			yOne = yOne*signY;
		}
	}
}

void CCGWorkView::chooseAndPutPoints(int* signX, int* signY, int* xOne, int* xTwo, int* yOne, int* yTwo, int x1, int y1, int x2, int y2)
{
	if (x2 >= x1 && y2 >= y1)
	{
		*signX = 1;
		*signY = 1;
		*xOne = x1;
		*yOne = y1;
		*xTwo = x2;
		*yTwo = y2;
	}
	else
	{
		if (x2 < x1 && y2 < y1)
		{
			*signX = 1;
			*signY = 1;
			*xOne = x2;
			*yOne = y2;
			*xTwo = x1;
			*yTwo = y1;
		}
		else{
			*signX = 1;
			*signY = 1;
			*xOne = x1;
			*yOne = y1;
			*xTwo = x2;
			*yTwo = y2;
			if (x2 < x1){
				*signX = -1;
			}
			if (y2 < y1){
				*signY = -1;
			}
		}
	}
}