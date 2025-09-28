#ifndef SG_RENDERER_EVENT_H
#define SG_RENDERER_EVENT_H
#include <cstdint>

namespace Context::Renderer::Event {

struct SwapchainRecreateEvent  {
	int32_t width_;
	int32_t height_;
};

struct RenderFrameEvent {
	uint64_t frame_id;
	double delta_time;
};




} //namespace Context::Renderer::Event

#endif
