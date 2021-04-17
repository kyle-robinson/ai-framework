#pragma once
#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "GraphicsResource.h"
#include "Logging/ErrorLogger.h"

namespace Bind
{
	class Rasterizer : public GraphicsResource
	{
	public:
		Rasterizer( Graphics& gfx )
		{
			try
			{
				CD3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
				rasterizerDesc.MultisampleEnable = TRUE;
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.CullMode = D3D11_CULL_BACK;
				HRESULT hr = GetDevice( gfx )->CreateRasterizerState( &rasterizerDesc, pRasterizer.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create rasterizer state!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->RSSetState( pRasterizer.Get() );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	};
}

#endif