#pragma once

#include "./task_runloop.hpp"

namespace TaskManager
{

template <typename LoopType>
template <typename... Args>
RunLoop<LoopType>::RunLoop(Args&&... _args) noexcept(std::is_nothrow_constructible<LoopType, Args...>::value)
    : m_loop{std::make_shared<LoopType>(std::forward<Args>(_args)...)}
{
}

template <typename LoopType>
RunLoop<LoopType>::RunLoop(const RunLoop& _other) noexcept
    : AbstTask{_other},
      m_loop{_other.m_loop}
{
}
template <typename LoopType>
    RunLoop<LoopType>& RunLoop<LoopType>::operator=(const RunLoop& _other) & noexcept
{
    AbstTask::operator=(_other);
    m_loop = _other.m_loop;
    return *this;
}
template <typename LoopType>
RunLoop<LoopType>::RunLoop(RunLoop&& _other) noexcept
    : AbstTask{std::move(_other)},
      m_loop{std::move(_other.m_loop)}
{
    _other.m_loop = nullptr;
}
template <typename LoopType>
    RunLoop<LoopType>& RunLoop<LoopType>::operator=(RunLoop&& _other) & noexcept
{
    AbstTask::operator=(std::move(_other));
    m_loop = std::move(_other.m_loop);
    _other.m_loop = nullptr;
    return *this;
}

template <typename LoopType>
void RunLoop<LoopType>::init()
{
    m_loop->start();
}
template <typename LoopType>
NextTask RunLoop<LoopType>::eval()
{
    return !m_loop->running();
}

template <typename LoopType>
void RunLoop<LoopType>::interrupt()
{
    m_loop->stop();
}

}  // namespace TaskManager
