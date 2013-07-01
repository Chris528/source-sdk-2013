
#include "BaseVSShader.h"

#include "PassThrough_vs30.inc" 
#include "post_sepia_ps30.inc"
 
BEGIN_VS_SHADER( Post_Sepia, "Help for Post_Sepia" )
 
	BEGIN_SHADER_PARAMS
	
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
			pShaderShadow->SetPixelShader( "post_sepia_ps30" );

			DefaultFog();
		}

		DYNAMIC_STATE
		{
			BindTexture( SHADER_SAMPLER0, FBTEXTURE, -1 );
		}
		Draw();
	}
END_SHADER