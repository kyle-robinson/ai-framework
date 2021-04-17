#pragma once
#ifndef SAMPLER_H
#define SAMPLER_H

#include "GraphicsResource.h"
#include "Logging/ErrorLogger.h"

namespace Bind
{
	class Sampler : public GraphicsResource
	{
	public:
		Sampler( Graphics& gfx )
		{
			try
			{
				std::string samplerType;

				CD3D11_SAMPLER_DESC samplerDesc( CD3D11_DEFAULT{} );
				samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

				HRESULT hr = GetDevice( gfx )->CreateSamplerState( &samplerDesc, pSampler.GetAddressOf() );
				COM_ERROR_IF_FAILED( hr, "Failed to create sampler state!" );
			}
			catch ( COMException& exception )
			{
				ErrorLogger::Log( exception );
				return;
			}
		}
		void Bind( Graphics& gfx ) noexcept override
		{
			GetContext( gfx )->PSSetSamplers( 0u, 1u, pSampler.GetAddressOf() );
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}

#endif