#pragma once
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <functional>
#include <memory>

namespace Scape 
{
	class Event 
	{
	public:
		enum Type
		{
			None = 0,
			WindowClose,
			WindowIconify,
			WindowMaximize,
			FramebufferResize,
			Key,
			Cursor,
			MouseButton,
			Scroll,
			DropFile
		};

		Event() {};
		virtual ~Event() = default;
		 Type GetType() { return _eventType; }

	protected:
		Type _eventType;
	};

	class WindowCloseEvent: public Event
	{
	public:
		WindowCloseEvent() { _eventType = Type::WindowClose; }
		~WindowCloseEvent() {}
	};

	class WindowIconifyEvent : public Event
	{
	public:
		WindowIconifyEvent(bool isIcon) : _iconified(isIcon) { _eventType = Type::WindowClose; }
		~WindowIconifyEvent() {}

		bool IsIconified() const { return _iconified; }
	private:
		bool _iconified;
	};

	class WindowMaximizeEvent : public Event
	{
	public:
		WindowMaximizeEvent(bool maximized) : _maximized(maximized) { _eventType = Type::WindowMaximize; }
		~WindowMaximizeEvent() {}
	private:
		bool _maximized;
	};

	class FrameBufferResizeEvent : public Event
	{
	public:
		FrameBufferResizeEvent(int width, int height) : _width(width), _height(height) { _eventType = Type::FramebufferResize; }
		~FrameBufferResizeEvent() {}

		int GetWidth() const { return _width; }
		int GetHeight() const { return _height; }
	private:
		int _width;
		int _height;
	};

	class KeyEvent : public Event
	{
	public:
		KeyEvent(int key, int scancode, int action, int mods) : _key(key), _scancode(scancode), _action(action), _mods(mods) { _eventType = Type::Key; }
		~KeyEvent() {}

		int GetKey() const { return _key; }
		int GetScanCode() const { return _scancode; }
		int GetAction() const { return _action; }
		int GetMods() const { return _mods; }
	private:
		int _key;
		int _scancode;
		int _action;
		int _mods;
	};

	class CursorEvent : public Event
	{
	public:
		CursorEvent(int x, int y) : _x(x), _y(y) { _eventType = Type::Cursor; }
		~CursorEvent() {}

		int GetX() const { return _x; }
		int GetY() const { return _y; }
	private:
		int _x;
		int _y;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int button, int action, int mods) : _button(button), _action(action), _mods(mods) { _eventType = Type::MouseButton; }
		~MouseButtonEvent() {}

		int GetButton() const { return _button; }
		int GetAction() const { return _action; }
		int GetMods() const { return _mods; }
	private:
		int _button;
		int _action;
		int _mods;
	};

	class ScrollEvent : public Event
	{
	public:
		ScrollEvent(int xOffset, int yOffset) : _xOffset(xOffset), _yOffset(yOffset) { _eventType = Type::Scroll; }
		~ScrollEvent() {}

		int GetOffsetX() const { return _xOffset; }
		int GetOffsetY() const { return _yOffset; }
	private:
		int _xOffset;
		int _yOffset;
	}; 
	
	class DropEvent : public Event
	{
	public:
		DropEvent(std::vector<std::string> paths) : _paths(paths) { _eventType = Type::DropFile; }
		~DropEvent() {}

		std::vector<std::string> GetPaths() { return _paths; }
	private:
		std::vector<std::string> _paths;
	};

	class EventDispatcher
	{
	public:
		~EventDispatcher() {};

		static EventDispatcher* Instance();

		bool EventsInQueue();
		void EnqueueEvent(std::shared_ptr<Event> event);
		std::shared_ptr<Event> DispatchNextEvent();
	protected:
		EventDispatcher() 
		{
			_eventQueue = std::queue<std::shared_ptr<Event>>();
		};

		static EventDispatcher* _instance;

		std::queue<std::shared_ptr<Event>> _eventQueue;
	};

};