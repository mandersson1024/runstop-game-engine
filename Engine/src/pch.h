//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>

#if defined(NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_5.h>
#endif

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <format>

#include <stdio.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "Engine/EngineAssert.h"

namespace DX
{
    /*
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
    */

    inline void AssertIfFailed(HRESULT result)
    {
        ENGINE_ASSERT(!FAILED(result), "");
    }
}

#include "Engine/Logger.h"
#include "Engine/Assets/AssetPaths.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/Event.h"
#include "Engine/Application/ApplicationEvent.h"
#include "Engine/Application/KeyEvent.h"
#include "Engine/Application/MouseEvent.h"
#include "Engine/Utils/MathUtils.h"
#include "Engine/Utils/StringUtils.h"
#include "Engine/Utils/TextureUtils.h"
#include "Engine/Utils/TimeLog.h"
#include "Engine/Utils/Json.h"

