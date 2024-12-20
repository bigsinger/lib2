// EnBitmap.cpp: implementation of the CEnBitmapEx class (c) daniel godson 2002.
//
// credits: Peter Hendrix's CPicture implementation for the original IPicture code 
//          Yves Maurer's GDIRotate implementation for the idea of working directly on 32 bit representations of bitmaps 
//          Karl Lager's 'A Fast Algorithm for Rotating Bitmaps' 
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnBitmapex.h"
#include "imageprocessors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnBitmapEx::CEnBitmapEx(COLORREF crBkgnd) : CEnBitmap(crBkgnd)
{

}

CEnBitmapEx::~CEnBitmapEx()
{

}

BOOL CEnBitmapEx::LoadSysBitmap(UINT nID)
{
	if (LoadBitmap(nID))
		return RemapSysColors();

	return FALSE;
}

BOOL CEnBitmapEx::RotateImage(int nDegrees, BOOL bEnableWeighting)
{
	CImageRotator proc(nDegrees, bEnableWeighting);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ShearImage(int nHorz, int nVert, BOOL bEnableWeighting)
{
	CImageShearer proc(nHorz, nVert, bEnableWeighting);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::GrayImage(COLORREF crMask)
{
	CImageGrayer proc;
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::BlurImage(int nAmount)
{
	CImageBlurrer proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::SharpenImage(int nAmount)
{
	CImageSharpener proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ResizeImage(double dFactor)
{
	CImageResizer proc(dFactor);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::LightenImage(double dAmount, COLORREF crMask)
{
	CImageLightener proc(dAmount);
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::FlipImage(BOOL bHorz, BOOL bVert)
{
	CImageFlipper proc(bHorz, bVert);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::NegateImage()
{
	CImageNegator proc;
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ReplaceColor(COLORREF crFrom, COLORREF crTo)
{
	CColorReplacer proc(crFrom, crTo);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ColorizeImage(COLORREF color)
{
	CImageColorizer proc(color);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ContrastImage(int nAmount)
{
	CImageContraster proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::TintImage(COLORREF color, int nAmount, COLORREF crMask)
{
	CImageTinter proc(color, nAmount);
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::RemapSysColors()
{
	CImageSysColorMapper proc;
	return ProcessImage(&proc);
}
