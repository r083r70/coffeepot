
#include "platform.h"

#if WITH_NOTIFYICON
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#include "resources.h"

#include <cassert>
#include <commctrl.h>
#include <shellapi.h>

namespace coffeepot
{
	constexpr u_int WMAPP_NOTIFYCALLBACK = WM_APP + 100;
	constexpr u_int g_NotifyIconId = 42069;

	WindowsPlatform* g_WindowsPlatform = nullptr;

	LRESULT CALLBACK WndProc(HWND window, u_int message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WMAPP_NOTIFYCALLBACK)
		{
			assert(g_WindowsPlatform != nullptr);
			g_WindowsPlatform->HandleNotifyIconAction(lParam);
		}

		return DefWindowProc(window, message, wParam, lParam);
	}

	WindowsPlatform::WindowsPlatform(const HINSTANCE& instance)
		: BasePlatform()
		, m_Instance(instance)
	{ }

	void WindowsPlatform::init()
	{
		const wchar_t g_WindowClassName[] = L"WindowClass";
		const wchar_t g_Title[] = L"Coffeepot";

		g_WindowsPlatform = this;

		CreateWindowClass(g_WindowClassName);
		m_WindowHandle = CreatePlatformWindow(g_WindowClassName, g_Title);
	}

	void WindowsPlatform::deinit()
	{
		DestroyWindow(m_WindowHandle);
	}

	void WindowsPlatform::tick()
	{
		if (GetMessage(&m_Message, nullptr, 0, 0))
		{
			TranslateMessage(&m_Message);
			DispatchMessage(&m_Message);
		}
	}

	bool WindowsPlatform::onWindowClosed()
	{
		return CreateNotifyIcon();
	}

	bool WindowsPlatform::onWindowOpened()
	{
		return DeleteNotifyIcon();
	}

	bool WindowsPlatform::HandleNotifyIconAction(LPARAM action)
	{
		if (LOWORD(action) != NIN_SELECT)
			return false;

		// Create Event (?)
		return true;
	}

	void WindowsPlatform::CreateWindowClass(const wchar_t* windowClassName)
	{
		WNDCLASSEX windowClass = { sizeof(windowClass) };
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = m_Instance;
		windowClass.lpszClassName = windowClassName;
		RegisterClassEx(&windowClass);
	}

	HWND WindowsPlatform::CreatePlatformWindow(const wchar_t* windowClassName, const wchar_t* windowTitle)
	{
		return CreateWindow(windowClassName, windowTitle, 0, 0, 0, 0, 0, nullptr, nullptr, m_Instance, nullptr);
	}

	bool WindowsPlatform::CreateNotifyIcon()
	{
		wchar_t const tipText[] = L"Coffeepot";

		NOTIFYICONDATA notifyIconData{ sizeof(notifyIconData) };
		notifyIconData.hWnd = m_WindowHandle;
		notifyIconData.uID = g_NotifyIconId;

		notifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
		LoadIconMetric(m_Instance, MAKEINTRESOURCE(IDI_COFFEEPOT), LIM_SMALL, &notifyIconData.hIcon);
		wcscpy_s(notifyIconData.szTip, tipText);
		notifyIconData.uCallbackMessage = WMAPP_NOTIFYCALLBACK;

		Shell_NotifyIcon(NIM_ADD, &notifyIconData);

		// Version
		notifyIconData.uVersion = NOTIFYICON_VERSION_4;
		return Shell_NotifyIcon(NIM_SETVERSION, &notifyIconData);
	}

	bool WindowsPlatform::DeleteNotifyIcon()
	{
		NOTIFYICONDATA notifyIconData{ sizeof(notifyIconData) };
		notifyIconData.hWnd = m_WindowHandle;
		notifyIconData.uID = g_NotifyIconId;

		return Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	}
}
#endif
