#include "core/async/coroutine/corutine_base.h"
#include "core/async/coroutine/cortutine_awaitable.h"


using namespace Core::Async;

//Generator
inline CoroutineGenerator<int,CoroutinePolicy::Generator> task()
{
	int a = 0, b = 1;
	while (a <= 10)
	{
		co_yield a;
		a++;
	}
	co_return;
};

//Wait
struct IntReader : public AwaitReader<IntReader,int>{
    bool await_ready() const noexcept{
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) {
        //Async thread operator [......]
    }
};




inline CoroutineGenerator<int,CoroutinePolicy::Await> computeSum() {
	IntReader reader1;
	int total = co_await reader1;

	IntReader reader2;
	total += co_await reader2;

	IntReader reader3;
	total += co_await reader3;

	co_return total;
}




