#ifndef SG_VK_TOOLS_READ_SHADER_H
#define SG_VK_TOOLS_READ_SHADER_H
#include <string_view>
#include <vector>

namespace Driver::Vulkan::Tools {
std::vector<char> ReadShaderFile(std::string_view);
auto ReadShaderFileAsync(std::string_view);


} //namespace Driver::Vulkan::Tools

#endif