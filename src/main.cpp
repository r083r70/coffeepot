
#include "core/events.h"
#include "core/imguiclient.h"
#include "core/log.h"
#include "core/window.h"

const char* title = "Coffeepot";
bool bShowDemo = true;
bool bShouldClose = false;

int32_t width = 640, height = 360;

void onEvent(coffeepot::Event& event)
{
    switch (event.getType())
    {
    case coffeepot::EventType::WindowClosed:
        bShouldClose = true;
        break;
    case coffeepot::EventType::WindowResized: {
        auto windowResizedEvent = static_cast<coffeepot::WindowResizedEvent&>(event);
        width = windowResizedEvent.m_Width;
        height = windowResizedEvent.m_Height;
    } break;
    case coffeepot::EventType::Char:
        CP_DEBUG("Char");
        break;
    }
}

int main(void)
{
    coffeepot::log::init();

    coffeepot::Window window;
    if (!window.init(title, width, height))
    {
        CP_FATAL("Failed to init Window");
        return -1;
    }

    window.setEventCallback(&onEvent);

    coffeepot::ImGuiClient imguiClient;
    if (!imguiClient.init(window))
    {
        CP_FATAL("Failed to init ImGuiClient");
        return -1;
    }

    while (!bShouldClose)
    {
        window.tick();

        imguiClient.preTick(width, height);
        imguiClient.tick(bShowDemo);
        imguiClient.postTick();
    }

    imguiClient.deinit();
    window.deinit();

    return 0;
}