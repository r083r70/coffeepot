#pragma once

#include "events.h"

#include <deque>
#include <vector>

namespace coffeepot
{
	class EventSubscriber
	{
	public:
		virtual bool onEvent(const Event& event) = 0;
	};

	class EventDispatcher
	{
	public:
		static EventDispatcher* get() { return s_Instance; }

		void init();
		void deinit();
		void tick();

		void dispatchEvent(const Event& event);

		bool hasEvents() const { return !events.empty(); }
		void createEvent(EventType eventType, void* eventData = nullptr);
		
		const Event& peekFrontEvent();
		void dequeueFrontEvent();

		Event dequeueEvent();

		void subscribe(EventSubscriber* subscriber);
		void unsubscribe(EventSubscriber* subscriber);

	private:
		static EventDispatcher* s_Instance;

		std::deque<Event> events;
		std::vector<EventSubscriber*> subscribers;
	};
}
