
#include "BaseVSShader.h"

#include "PassThrough_vs30.inc" 
#include "post_vignette_ps30.inc"

ConVar dev_post_vignette_outer_radius_override("dev_post_vignette_outer_radius_override", "-1.0");
ConVar dev_post_vignette_inner_radius_override("dev_post_vignette_inner_radius_override", "-1.0");
 
BEGIN_VS_SHADER( Post_Vignette, "Help for Post_Vignette" )
 
	BEGIN_SHADER_PARAMS
	
	SHADER_PARAM(INNER_RADIUS, SHADER_PARAM_TYPE_FLOAT, ".38", "")
	SHADER_PARAM(OUTER_RADIUS, SHADER_PARAM_TYPE_FLOAT, ".45", "")
	SHADER_PARAM( FBTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "_rt_FullFrameFB", "" )

	END_SHADER_PARAMS
  
	SHADER_FALLBACK
	{
		return 0;
	}
 
	SHADER_INIT_PARAMS()
	{
		if ( !params[ FBTEXTURE ]->IsDefined() )
		{
			params[ FBTEXTURE ]->SetStringValue( "_rt_FullFrameFB" );
		}
	}

	SHADER_INIT
	{
		if( params[FBTEXTURE]->IsDefined() )
		{
			LoadTexture( FBTEXTURE );
		}
	}
 
	SHADER_DRAW
	{
		SHADOW_STATE
		{
			pShaderShadow->EnableDepthWrites( false );
			
			pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
 
			int fmt = VERTEX_POSITION;

			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0 ); 
			pShaderShadow->SetVertexShader( "PassThrough_vs30", 0 ); 
			pShaderShadow->SetPixelShader( "post_vignette_ps30" );

			DefaultFog();
		}

		DYNAMIC_STATE
		{
			
			float innerRadius = params[INNER_RADIUS]->GetFloatValue();
			float outerRadius = params[OUTER_RADIUS]->GetFloatValue();
			if( dev_post_vignette_outer_radius_override.GetFloat() != -1.0f ) outerRadius = dev_post_vignette_outer_radius_override.GetFloat();
			if( dev_post_vignette_inner_radius_override.GetFloat() != -1.0f ) innerRadius = dev_post_vignette_inner_radius_override.GetFloat();

			pShaderAPI->SetPixelShaderConstant(0, &innerRadius);
			pShaderAPI->SetPixelShaderConstant(1, &outerRadius);
			
			BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );
		}
		Draw();
	}
END_SHADER