
#include "eventdispatcher.h"

namespace coffeepot
{
	coffeepot::EventDispatcher* EventDispatcher::s_Instance;
	
	void EventDispatcher::init()
	{
		s_Instance = this;
		events.clear();
		subscribers.clear();
	}

	void EventDispatcher::deinit()
	{
		events.clear();
		subscribers.clear();
	}

	void EventDispatcher::tick()
	{
		while (hasEvents())
		{
			const Event& event = peekFrontEvent();
			dispatchEvent(event);
			dequeueFrontEvent();
		}
	}

	void EventDispatcher::dispatchEvent(const Event& event)
	{
		for (auto it = subscribers.end(); it != subscribers.begin(); )
		{
			EventSubscriber* subscriber = *(--it);
			if (subscriber->onEvent(event))
				break;
		}
	}

	void EventDispatcher::createEvent(EventType eventType, void* eventData /*= nullptr*/)
	{
		events.emplace_back(eventType, eventData);
	}

	const Event& EventDispatcher::peekFrontEvent()
	{
		return events.front();
	}

	void EventDispatcher::dequeueFrontEvent()
	{
		events.pop_front();
	}

	void EventDispatcher::subscribe(EventSubscriber* subscriber)
	{
		subscribers.push_back(subscriber);
	}

	void EventDispatcher::unsubscribe(EventSubscriber* subscriber)
	{
		auto elem = std::remove(subscribers.begin(), subscribers.end(), subscriber);
	}
}
