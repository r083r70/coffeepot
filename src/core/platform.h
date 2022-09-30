#pragma once

#include "eventdispatcher.h"

#define WITH_NOTIFYICON CP_WINDOWS
#if WITH_NOTIFYICON
#include <windows.h >
#endif

namespace coffeepot
{
	class Event;

	class Platform : public EventSubscriber
	{
	public:
		Platform();

		void init();
		void deinit();
		void tick();

		virtual bool onEvent(const Event& event) override;

#if WITH_NOTIFYICON
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