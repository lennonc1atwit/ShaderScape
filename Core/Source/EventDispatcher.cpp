#include <Event.h>

namespace Scape {
    EventDispatcher* EventDispatcher::_instance = nullptr;

    EventDispatcher* EventDispatcher::Instance()
    {
        if (EventDispatcher::_instance == nullptr)
            _instance = new EventDispatcher();

        return _instance;
    }
    bool EventDispatcher::EventsInQueue()
    {
        return !_eventQueue.empty();
    }

    void EventDispatcher::EnqueueEvent(std::shared_ptr<Event> event)
    {
        _eventQueue.push(event);
    }

    std::shared_ptr<Event> EventDispatcher::DispatchNextEvent()
    {
        std::shared_ptr<Event> event = _eventQueue.front();
        _eventQueue.pop();
        return event;
    }
}