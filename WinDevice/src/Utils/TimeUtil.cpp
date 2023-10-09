#include "TimeUtil.h"
#include "spdlog/spdlog.h"


template <typename Func>
void TimeUtil<Func>::CalExecuteTime(Func func)
{
    // 测量函数调用的耗时
    const auto start = std::chrono::high_resolution_clock::now();

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    spdlog::info("Function call duration: {0} ms", duration.count());
}
