#ifndef LUD_TIME_HEADER
#define LUD_TIME_HEADER

#include <chrono>
#include <format>
#include <print>

namespace Lud {
class Timer
{
    using ClockT = std::chrono::steady_clock;

public:
    using uT = std::chrono::duration<double, std::micro>;
    using mT = std::chrono::duration<double, std::milli>;
    using sT = std::chrono::duration<double, std::ratio<1, 1>>;

    Timer(const std::string_view name = "", bool start = true);
    virtual ~Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    void Stop();

    void Start() noexcept;

    virtual std::string ToString() const;

    mT GetM() const noexcept { return std::chrono::duration_cast<mT>(m_total); }
    uT GetU() const noexcept { return std::chrono::duration_cast<uT>(m_total); }
    sT GetS() const noexcept { return std::chrono::duration_cast<sT>(m_total); }

protected:
    std::string m_name;
    std::chrono::time_point<ClockT> m_begin;

    uT m_total{};

    bool m_running;
};
} // namespace Lud

// IMPLEMENTATION
namespace Lud {

Timer::Timer(std::string_view name, bool start)
    : m_name(name)
    , m_running(start)
{
    if (start) [[likely]]
    {
        m_begin = ClockT::now();
    }
}

Timer::~Timer()
{
    if (m_running)
    {
        Stop();
        std::println("{}", this->ToString());
    }
}

void Timer::Stop()
{

    if (m_running) [[likely]]
    {
        auto end = ClockT::now();
        auto difference = end - m_begin;
        m_total += std::chrono::duration_cast<uT>(difference);
        m_running = false;
    }
}

void Timer::Start() noexcept
{
    if (!m_running) [[likely]]
    {
        m_running = true;
        m_begin = ClockT::now();
    }
}

std::string Timer::ToString() const
{
    using namespace std::chrono_literals;
    std::string result;
    if (!m_name.empty())
    {
        std::format_to(std::back_inserter(result), "[TIMER] : {}\n", m_name);
    }
    if (m_total > 1s)
    {
        auto st = std::chrono::duration_cast<sT>(m_total);
        std::format_to(std::back_inserter(result), "   elapsed: {}", st);
    }
    else if (m_total > 1ms)
    {
        auto mt = std::chrono::duration_cast<mT>(m_total);
        std::format_to(std::back_inserter(result), "   elapsed: {}", mt);
    }
    else
    {
        std::format_to(std::back_inserter(result), "   elapsed: {}", m_total);
    }
    return result;
}

} // namespace Lud

#endif //! LUD_TIME_HEADER
