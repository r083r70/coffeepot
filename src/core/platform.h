#pragma once

#define WITH_NOTIFYICON (CP_WINDOWS && NDEBUG)
#if WITH_NOTIFYICON
#define IF_NOTIFYICON(code) code
#define IF_ELSE_NOTIFYICON(ifCode, elseCode) ifCode
#else
#define IF_NOTIFYICON(code)
#define IF_ELSE_NOTIFYICON(ifCode, elseCode) elseCode
#endif

#if WITH_NOTIFYICON
#include <windows.h >
#endif

namespace coffeepot
{
	class BasePlatform
	{
	public:
		BasePlatform() = default;
		virtual ~BasePlatform() = default;

		virtual void init() {}
		virtual void deinit() {}
		virtual void tick() {}

		virtual bool onWindowClosed() { return false; }
		virtual bool onWindowOpened() { return false; }
	};

#if WITH_NOTIFYICON
	class WindowsPlatform : public BasePlatform
	{
	public:
		WindowsPlatform(const HINSTANCE& instance);

		virtual void init() override;
		virtual void deinit() override;
		virtual void tick() override;

		virtual bool onWindowClosed() override;
		virtual bool onWindowOpened() override;

		bool HandleNotifyIconAction(LPARAM action);

	private:
		void CreateWindowClass(const wchar_t* windowClassName);
		HWND CreatePlatformWindow(const wchar_t* windowClassName, const wchar_t* windowTitle);

		bool CreateNotifyIcon();
		bool DeleteNotifyIcon();

	private:
		static WindowsPlatform* s_Instance;

		HINSTANCE m_Instance;
		HWND m_WindowHandle;
		MSG m_Message;
	};
#endif
}