//========= Copyright © 1996-2012, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "ppshaderrender.h"
#include "materialsystem/IMaterialVar.h"

#include "clienteffectprecachesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CLIENTEFFECT_REGISTER_BEGIN( PrecacheVignette )
CLIENTEFFECT_MATERIAL( "postprocess/vignette" )
CLIENTEFFECT_MATERIAL( "postprocess/sepia" )
CLIENTEFFECT_REGISTER_END()

ConVar dev_post_vignette("dev_post_vignette","0");
ConVar dev_post_sepia("dev_post_sepia","0");

void Vignette( const CViewSetup &view );
void Sepia( const CViewSetup &view );
void draw_pp_shader( const CViewSetup &view, IMaterial *pMaterial);

void draw_pp_shaders( const CViewSetup &view )
{
	Vignette( view );
	Sepia( view );
}

void Vignette( const CViewSetup &view )
{
	if (dev_post_vignette.GetInt() != 1)
		return;

	static IMaterial *pMaterialVignette = materials->FindMaterial( "postprocess/vignette", TEXTURE_GROUP_CLIENT_EFFECTS, true );

	if ( IsErrorMaterial( pMaterialVignette ))
		return;
	
	draw_pp_shader(view, pMaterialVignette);
}

void Sepia( const CViewSetup &view )
{
	if (dev_post_sepia.GetInt() != 1)
		return;

	static IMaterial *pMaterialSepia = materials->FindMaterial( "postprocess/sepia", TEXTURE_GROUP_CLIENT_EFFECTS, true );

	if ( IsErrorMaterial( pMaterialSepia ))
		return;
	
	draw_pp_shader(view, pMaterialSepia);
}

void draw_pp_shader( const CViewSetup &view, IMaterial *pMaterial)
{
	int x = view.x;
	int y = view.y;
	int w = view.width;
	int h = view.height;

	Rect_t actualRect;
	UpdateScreenEffectTexture( 0, x, y, w, h, false, &actualRect);
	int u1 = actualRect.x;
	int v1 = actualRect.y;
	int u2 = actualRect.x+actualRect.width-1;
	int v2 = actualRect.y+actualRect.height-1;

	ITexture *pTexture = GetFullFrameFrameBufferTexture( 0 );
	int aw = pTexture->GetActualWidth();
	int ah = pTexture->GetActualHeight();

	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->DrawScreenSpaceRectangle( pMaterial, x, y, w, h, u1, v1, u2, v2, aw, ah );
	pRenderContext.SafeRelease();
}