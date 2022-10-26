#pragma once

#include "core/layer.h"

#define WITH_NOTIFYICON CP_WINDOWS
#if WITH_NOTIFYICON
#include <windows.h >
#endif

namespace coffeepot
{
	class Event;

	class Platform : public kettle::Layer
	{
	public:
		Platform();

		virtual void start() override;
		virtual void stop() override;

		virtual void tick() override;

#if WITH_NOTIFYICON
		virtual bool onWindowHidden() override;
		bool HandleNotifyIconAction(LPARAM action);

	private:
		void CreateWindowClass(const wchar_t* windowClassName);
		HWND CreatePlatformWindow(const wchar_t* windowClassName, const wchar_t* windowTitle);

		bool CreateNotifyIcon();
		bool DeleteNotifyIcon();

	private:
		HINSTANCE m_Instance;
		HWND m_WindowHandle;
		MSG m_Message;

		bool m_NotifyIconVisible;
#endif
	};
}