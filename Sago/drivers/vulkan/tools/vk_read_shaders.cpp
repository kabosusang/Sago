#include "vk_read_shaders.h"

#include <fstream>
#include <future>

#include "core/io/log/log.h"

namespace Driver::Vulkan::Tools {
std::vector<char> ReadShaderFile(std::string_view filename) {
	std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		LogErrorDetaill("[File][Open] File Not Open! :{}",filename);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer; //rvo
}
std::future<std::vector<char>> ReadShaderFileAsync(std::string_view filename) {
	return std::async(std::launch::async, [filename]() {
		std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			LogErrorDetaill("[File][Open] File Not Open!: {}",filename);
		}

		size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	});
}

} //namespace Driver::Vulkan::Tools