
#include "platform.h"

#if WITH_NOTIFYICON
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#include "core/app.h"
#include "resources.h"

#include <cassert>
#include <commctrl.h>
#include <shellapi.h>

namespace Windows
{
	coffeepot::Platform* g_Platform = nullptr;
	constexpr u_int WMAPP_NOTIFYCALLBACK = WM_APP + 100;

	LRESULT CALLBACK WndProc(HWND window, u_int message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WMAPP_NOTIFYCALLBACK)
		{
			assert(g_Platform != nullptr);
			g_Platform->HandleNotifyIconAction(lParam);
		}

		return DefWindowProc(window, message, wParam, lParam);
	}
}

namespace coffeepot
{
	constexpr u_int g_NotifyIconId = 42069;

	Platform::Platform()
	{
		m_Instance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
		Windows::g_Platform = this;
	}

	void Platform::start()
	{
		const wchar_t g_WindowClassName[] = L"WindowClass";
		const wchar_t g_Title[] = L"Coffeepot";

		CreateWindowClass(g_WindowClassName);
		m_WindowHandle = CreatePlatformWindow(g_WindowClassName, g_Title);

		m_NotifyIconVisible = false;
	}

	void Platform::stop()
	{
		DestroyWindow(m_WindowHandle);
		Windows::g_Platform = nullptr;
	}

	void Platform::tick()
	{
		if (!m_NotifyIconVisible)
			return;

		if (PeekMessage(&m_Message, m_WindowHandle, 0, 0, 0))
		{
			GetMessage(&m_Message, m_WindowHandle, 0, 0);
			TranslateMessage(&m_Message);
			DispatchMessage(&m_Message);
		}
	}

	bool Platform::onWindowHidden()
	{
		CreateNotifyIcon();
		return true;
	}

	bool Platform::HandleNotifyIconAction(LPARAM action)
	{
		if (LOWORD(action) != NIN_SELECT)
			return false;

		kettle::App::get()->showWindow();
		return true;
	}

	void Platform::CreateWindowClass(const wchar_t* windowClassName)
	{
		WNDCLASSEX windowClass = { sizeof(windowClass) };
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = Windows::WndProc;
		windowClass.hInstance = m_Instance;
		windowClass.lpszClassName = windowClassName;
		RegisterClassEx(&windowClass);
	}

	HWND Platform::CreatePlatformWindow(const wchar_t* windowClassName, const wchar_t* windowTitle)
	{
		return CreateWindow(windowClassName, windowTitle, 0, 0, 0, 0, 0, nullptr, nullptr, m_Instance, nullptr);
	}

	bool Platform::CreateNotifyIcon()
	{
		wchar_t const tipText[] = L"Coffeepot";

		NOTIFYICONDATA notifyIconData{ sizeof(notifyIconData) };
		notifyIconData.hWnd = m_WindowHandle;
		notifyIconData.uID = g_NotifyIconId;

		notifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
		LoadIconMetric(m_Instance, MAKEINTRESOURCE(IDI_COFFEEPOT), LIM_SMALL, &notifyIconData.hIcon);
		wcscpy_s(notifyIconData.szTip, tipText);
		notifyIconData.uCallbackMessage = Windows::WMAPP_NOTIFYCALLBACK;

		bool result = Shell_NotifyIcon(NIM_ADD, &notifyIconData) == TRUE;

		// Version
		notifyIconData.uVersion = NOTIFYICON_VERSION_4;
		result &= (Shell_NotifyIcon(NIM_SETVERSION, &notifyIconData) == TRUE);

		m_NotifyIconVisible = result;
		return result;
	}

	bool Platform::DeleteNotifyIcon()
	{
		NOTIFYICONDATA notifyIconData{ sizeof(notifyIconData) };
		notifyIconData.hWnd = m_WindowHandle;
		notifyIconData.uID = g_NotifyIconId;

		const bool result = Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
		m_NotifyIconVisible = false;

		return result;
	}
}
#else
namespace coffeepot
{
	Platform::Platform() { }

	void Platform::start() { }
	void Platform::stop() { }

	void Platform::tick() { }
}
#endif
