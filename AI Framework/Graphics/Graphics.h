#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Shaders.h"
#include "ImGuiManager.h"
#include <memory>

namespace Bind
{
	class DepthStencil;
	class RenderTarget;
	class Rasterizer;
	class SwapChain;
	class Viewport;
	class Sampler;
}

class Graphics
{
	friend class GraphicsResource;
public:
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame();
	void EndFrame();

	ID3D11Device* GetDevice() const noexcept { return device.Get(); }
	ID3D11DeviceContext* GetContext() const noexcept { return context.Get(); }

	UINT GetWidth() const noexcept { return windowWidth; };
	UINT GetHeight() const noexcept { return windowHeight; };
	ImGuiManager GetImGuiManager() const noexcept { return imgui; }
private:
	void InitializeDirectX( HWND hWnd );
	bool InitializeShaders();

	// Device/Context
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// Pipeline States
	std::shared_ptr<Bind::Sampler> sampler;
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::Rasterizer> rasterizer;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> renderTarget;

	// Shaders
	VertexShader vertexShader;
	PixelShader pixelShader;

	// Local Variables
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
};

#endif