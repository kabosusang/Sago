#include "core/Core.h"



int main() {
    LogInfo("Hello {}orld", 'W');
	LogError("Hello {}orld", 'W');
	LogWarring("Hello {}orld", 'W');

    LogInfoDetaill("Hello {}orld", 'W');
	LogErrorDetaill("Hello {}orld", 'W');
	LogWarringDetaill("Hello {}orld", 'W');


	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}