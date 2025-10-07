#ifndef SG_ECS_GROUP_H
#define SG_ECS_GROUP_H
#include "entity.h"

#include <vector>

namespace SECS{
class registry;

template <typename... Components>
class basic_group {
private:
	registry& owner_;
	std::vector<entity> entities_;
	bool dirty_ = true;

	void rebuild_if_needed();

public:
	basic_group(registry& reg);

	void mark_dirty();

	template <typename Func>
	void each(Func func);

	template <typename Compare>
	void sort(Compare comp);

	template <typename Component, typename Compare>
	void sort_by_component(Compare comp);

	const std::vector<entity>& get_entities();
	std::size_t size();
	bool empty();
};







}






#endif