#ifndef SG_ECS_ENTITY_H
#define SG_ECS_ENTITY_H
#include <cstdint>
#include <limits>

namespace SECS{
// low   48   id
// hight 16 version
using entity = std::uint64_t;

static constexpr entity null = std::numeric_limits<entity>::max();
static constexpr entity tombstone = null - 1;

constexpr std::uint16_t ENTITY_VERSION_BITS = 16;
constexpr std::uint64_t ENTITY_ID_MASK = 0x0000FFFFFFFFFFFF;

inline constexpr std::uint64_t entity_id(entity e) noexcept {
    return e & ENTITY_ID_MASK;
}

inline constexpr std::uint16_t entity_version(entity e) noexcept {
    return static_cast<std::uint16_t>(e >> (64 - ENTITY_VERSION_BITS));
}

inline constexpr entity make_entity(std::uint64_t id, std::uint16_t version) noexcept {
    return (static_cast<entity>(version) << (64 - ENTITY_VERSION_BITS)) | (id & ENTITY_ID_MASK);
}


#ifdef NDEBUG
    #define SECS_ASSERT(condition, message) ((void)0)
#else
    #define SECS_ASSERT(condition, message)                                      \
        do {                                                                     \
            if (!(condition)) {                                                  \
                std::terminate();                                                \
            }                                                                    \
        } while (0)
#endif


}


#endif