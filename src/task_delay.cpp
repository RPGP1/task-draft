#include "task_delay.hpp"

namespace TaskManager
{

Delay::Delay(int _delay) noexcept
    : m_delay{_delay},
      m_count{0}
{
}

Delay::Delay(const Delay& _other) noexcept
    : AbstTask{_other},
      m_delay{_other.m_delay},
      m_count{0}
{
}
Delay& Delay::operator=(const Delay& _other) & noexcept
{
    AbstTask::operator=(_other);
    m_delay = _other.m_delay;
    m_count = 0;
    return *this;
}
Delay::Delay(Delay&& _other) noexcept
    : AbstTask{std::move(_other)},
      m_delay{_other.m_delay},
      m_count{0}
{
}
Delay& Delay::operator=(Delay&& _other) & noexcept
{
    AbstTask::operator=(std::move(_other));
    m_delay = _other.m_delay;
    m_count = 0;
    return *this;
}

void Delay::init() noexcept
{
    m_count = 0;
}
NextTask Delay::eval() noexcept
{
    return ++m_count > m_delay;
}

}  // namespace TaskManager
