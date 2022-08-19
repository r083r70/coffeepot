#pragma once

#include <functional>

namespace coffeepot
{
    enum class EventType
    {
        None,
		WindowClosed,
		WindowResized,
		NotifyIconInteracted
	};

    class Event
    {
    public:
        Event(EventType eventType, void* eventData) : m_Type(eventType), m_Data(eventData) {}
        ~Event() { delete m_Data; }

        EventType getType() const { return m_Type; }
		void* getRawData() const { return m_Data; }

        template<class T>
		T* getData() const { return static_cast<T*>(m_Data); }

	private:
		EventType m_Type;
		void* m_Data;
    };
    
    struct WindowResizedEventData
    {
        int32_t m_Width;
        int32_t m_Height;
	};
}