// FileAwaitable.h
#ifndef SG_COROUTINE_FILE_READ_H
#define SG_COROUTINE_FILE_READ_H

#include "core/async/coroutine/cortutine_awaitable.h"
#include "core/async/coroutine/corutine_base.h"

#include <fstream>
#include <future>

namespace Core::Async {

class FileReader : public AwaitReader<FileReader, std::vector<char>> {
public:
	FileReader(std::string_view filename, CompletionCallback callback = nullptr) :
			AwaitReader<FileReader, std::vector<char>>(std::move(callback)),
			filename_(filename) {}


	bool await_ready() const noexcept {
		return false;
	}

	void await_suspend(std::coroutine_handle<> handle) {
		future_ = std::async(std::launch::async, [this, handle]() {
			std::ifstream file(filename_.data(), std::ios::ate | std::ios::binary);
			if (!file.is_open()) {
				LogErrorDetail("[File][Open] File Not Open!: {}", filename_);
				notify_data_ready({}); //Null
				return;
			}

			size_t fileSize = file.tellg();
			std::vector<char> buffer(fileSize);
			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			notify_data_ready(std::move(buffer));
		});
	}

private:
	std::string filename_;
	std::future<void> future_;
};

} // namespace Core::Async

#endif