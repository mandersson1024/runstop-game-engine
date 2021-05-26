#include "pch.h"
#include "NativeWindow.h"
#include "Input.h"

namespace Engine
{

	NativeWindow* NativeWindow::nativeWindow_;

	NativeWindow::NativeWindow(HINSTANCE moduleHandle, const std::string& title, int width, int height)
		: moduleHandle_(moduleHandle)
		, title_(title)
		, width_(width)
		, height_(height)
		, inSizemove_(false)
		, inSuspend_(false)
		, minimized_(false)
		, fullscreen_(false)
	{

		RegisterClass();
		nativeWindow_ = this;
	}

	void NativeWindow::Spawn(INativeWindowListener* listener, int nCmdShow)
	{
		listener_ = listener;

		RECT rc = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		hwnd_ = CreateWindowEx(
			0,
			className_,
			title_.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			nullptr,
			nullptr,
			moduleHandle_,
			nullptr);

        ENGINE_ASSERT(hwnd_ != NULL, fmt::format("Error creating window (error code: {})", GetLastError()));

		ShowWindow(hwnd_, nCmdShow); // TODO: Change to SW_SHOWMAXIMIZED to default to fullscreen.
		//UpdateWindow(m_hwnd);
	}

	NativeWindow::~NativeWindow()
	{
		UnregisterClass();
	}

	void NativeWindow::RegisterClass()
	{
		WNDCLASSEX wcex = 
		{
            .cbSize = sizeof(WNDCLASSEXW),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = StaticWndProc,
            .hInstance = moduleHandle_,
            .hIcon = LoadIcon(moduleHandle_, "IDI_ICON"),
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
            .lpszClassName = className_,
            .hIconSm = LoadIcon(wcex.hInstance, "IDI_ICON"),
		};

		ATOM result = RegisterClassEx(&wcex);
        ENGINE_ASSERT(result != 0, fmt::format("Could not register class (error code: {})", GetLastError()));
	}

	void NativeWindow::UnregisterClass()
	{
		::UnregisterClass(className_, moduleHandle_);
	}

	void NativeWindow::SetTitle(const char* title)
	{
		SetWindowText(hwnd_, title);
	}

	void NativeWindow::PollEvents()
	{
		MSG message{};

		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	LRESULT NativeWindow::StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//static WindowsMessageMap messageMap;
		//GAME_LOG("NativeWindow::StaticWndProc", messageMap.Get(message));

		if (LRESULT result = nativeWindow_->WndProc(message, wParam, lParam))
		{
			return result;
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	
	LRESULT NativeWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (additionalWinProcHandler_)
		{
			if (additionalWinProcHandler_(hwnd_, msg, wParam, lParam))
			{
				return true;
			}
		}

		switch (msg)
		{
			case WM_CLOSE:
			{
				listener_->OnClose();
				break;
			}
			case WM_MOVE:
				listener_->OnMoved();
				break;
			case WM_SIZE:
				if (wParam == SIZE_MINIMIZED)
				{
					if (!minimized_)
					{
						minimized_ = true;
						if (!inSuspend_)
						{
							listener_->OnSuspending();
						}

						inSuspend_ = true;
					}
				}
				else if (minimized_)
				{
					minimized_ = false;
					if (inSuspend_)
					{
						listener_->OnResuming();
					}

					inSuspend_ = false;
				}
				else if (!inSizemove_)
				{
					listener_->OnSizeChanged(LOWORD(lParam), HIWORD(lParam));
				}
				break;

			case WM_ENTERSIZEMOVE:
				inSizemove_ = true;
				break;

			case WM_EXITSIZEMOVE:
			{
				inSizemove_ = false;
				RECT rc{};
				GetClientRect(hwnd_, &rc);
				listener_->OnSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
				break;
			}
			case WM_GETMINMAXINFO:
				if (lParam)
				{
					auto info = reinterpret_cast<MINMAXINFO*>(lParam);
					info->ptMinTrackSize.x = 320;
					info->ptMinTrackSize.y = 200;
				}
				break;

			case WM_ACTIVATEAPP:
			{
				DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);

				if (wParam)
				{
					listener_->OnActivated();
				}
				else
				{
					listener_->OnDeactivated();
				}
				break;
			}

			case WM_POWERBROADCAST:
			{
				switch (wParam)
				{
					case PBT_APMQUERYSUSPEND:
						if (!inSuspend_)
						{
							listener_->OnSuspending();
						}

						inSuspend_ = true;
						return TRUE;

					case PBT_APMRESUMESUSPEND:
						if (!minimized_)
						{
							if (inSuspend_)
							{
								listener_->OnResuming();
							}

							inSuspend_ = false;
						}
						return TRUE;
				}
				break;
			}
			case WM_KEYDOWN:
			{
				int keycode = (int)wParam;
				DirectX::Keyboard::ProcessMessage(msg, keycode, lParam);
				listener_->OnKeyDown(keycode, TestAutoRepeatBit(lParam));
				break;
			}
			case WM_KEYUP:
			{
				int keycode = (int)wParam;
				DirectX::Keyboard::ProcessMessage(msg, keycode, lParam);
				listener_->OnKeyUp(keycode);
				break;
			}
			case WM_CHAR:
			{
				int charcode = (int)wParam;
				listener_->OnKeyTyped(charcode, TestAutoRepeatBit(lParam));
				break;
			}
			case WM_SYSKEYDOWN:
			{
				int keycode = (int)wParam;
				DirectX::Keyboard::ProcessMessage(msg, keycode, lParam);
				listener_->OnKeyDown(keycode, TestAutoRepeatBit(lParam));

				if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
				{
					// Implements the classic ALT+ENTER fullscreen toggle
					if (fullscreen_)
					{
						SetWindowLongPtr(hwnd_, GWL_STYLE, WS_OVERLAPPEDWINDOW);
						SetWindowLongPtr(hwnd_, GWL_EXSTYLE, 0);

						ShowWindow(hwnd_, SW_SHOWNORMAL);
						SetWindowPos(hwnd_, HWND_TOP, 0, 0, 1280, 720, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
					}
					else
					{
						SetWindowLongPtr(hwnd_, GWL_STYLE, 0);
						SetWindowLongPtr(hwnd_, GWL_EXSTYLE, WS_EX_TOPMOST);

						SetWindowPos(hwnd_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
						ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
					}

					fullscreen_ = !fullscreen_;
				}
				break;
			}
			case WM_MENUCHAR:
			{
				// A menu is active and the user presses a key that does not correspond
				// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
				return MAKELRESULT(0, MNC_CLOSE);
			}
			case WM_INPUT:
			{
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
				break;
			}
			case WM_MOUSEMOVE:
			{
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
				auto state = Input::GetMouse().GetState();
				listener_->OnMouseMove(state.x, state.y);
				break;
			}
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
			{
				bool wasAnyButtonDown = Input::IsAnyMouseButtonDown();
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);

				int button = 0;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
				if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
				if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
				if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }

				if (!wasAnyButtonDown && ::GetCapture() == NULL)
				{
					::SetCapture(hwnd_);
				}

				listener_->OnMouseButtonDown(button);
				break;
			}
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);

				int button = 0;
				if (msg == WM_LBUTTONUP) { button = 0; }
				if (msg == WM_RBUTTONUP) { button = 1; }
				if (msg == WM_MBUTTONUP) { button = 2; }
				if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }

				if (!Input::IsAnyMouseButtonDown() && ::GetCapture() == hwnd_)
				{
					::ReleaseCapture();
				}

				listener_->OnMouseButtonUp(button);
				break;
			}
			case WM_MOUSEWHEEL:
			{
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);

				float delta = Input::ConsumeScrollWheelValue();
				listener_->OnMouseScroll(delta);
				break;
			}
			case WM_MOUSEHOVER:
			{
				DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
				break;
			}
		}

		return FALSE;
	}

}
