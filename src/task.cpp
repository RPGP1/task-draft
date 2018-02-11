#include "task.hpp"

namespace TaskManager
{

Task::Task(const Task& _other) noexcept
    : AbstTask{_other},
      m_function{_other.m_function}
{
}
Task& Task::operator=(const Task& _other) & noexcept
{
    AbstTask::operator=(_other);
    m_function = _other.m_function;
    return *this;
}
Task::Task(Task&& _other) noexcept
    : AbstTask{std::move(_other)},
      m_function{std::move(_other.m_function)}
{
    _other.m_function = nullptr;
}
Task& Task::operator=(Task&& _other) & noexcept
{
    AbstTask::operator=(std::move(_other));
    m_function = std::move(_other.m_function);
    _other.m_function = nullptr;
    return *this;
}

bool Task::eval()
{
    // std::functionはnullptrも格納できるので、チェック
    return m_function && m_function();
}

}  // namespace TaskManager
