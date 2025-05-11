#pragma once
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include "GeometryGenerator.h"
#include "UploadBuffer.hpp"
#include "stb_image.h"
#include "DDSTextureLoader.h"
#include "d3dUtil.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

namespace Engine {
	struct WindowInfo {
		int Wincx = 1920;
		int Wincy = 1080;
		bool isFullScreen = false;
		HWND windowHandle;
	};
}

const static UINT g_NumFrameResources = 3;

using namespace Engine;
using namespace std;