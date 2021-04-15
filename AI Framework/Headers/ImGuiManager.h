#pragma once
#ifndef IMGUIMANAGER_H
#define IMGUIMANAGER_H

#include <d3d11.h>

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();
	void Initialize( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context ) const noexcept;
	void BeginRender() const noexcept;
	void EndRender() const noexcept;
	void SpawnInstructionWindow() const noexcept;
private:
	void SetGreenTheme() const noexcept;
};

#endif