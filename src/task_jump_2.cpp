#include "task_jump.hpp"


namespace TaskManager
{

namespace Expr
{
    Jump::JumpManager::JumpManagerOperator::JumpManagerOperator(const std::shared_ptr<JumpManager>& _jump_manager, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : JumpIf{_jump_manager, _taskset},
          JumpBackIf{_jump_manager, _taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpManagerOperator(std::shared_ptr<JumpManager>&& _jump_manager, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : JumpIf{_jump_manager, _taskset},
          JumpBackIf{std::move(_jump_manager), std::move(_taskset)}
    {
    }


    Jump::JumpManager::JumpManagerOperator::JumpIfCondition::JumpIfCondition(const std::shared_ptr<JumpManager>& _jump_manager, int _priority, const std::function<bool()>& _func, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_priority{_priority},
          m_func{_func},
          m_taskset{_taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition::JumpIfCondition(const std::shared_ptr<JumpManager>& _jump_manager, int _priority, std::function<bool()>&& _func, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_priority{_priority},
          m_func{std::move(_func)},
          m_taskset{_taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition::JumpIfCondition(std::shared_ptr<JumpManager>&& _jump_manager, int _priority, const std::function<bool()>& _func, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : m_jump_manager{std::move(_jump_manager)},
          m_priority{_priority},
          m_func{_func},
          m_taskset{std::move(_taskset)}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition::JumpIfCondition(std::shared_ptr<JumpManager>&& _jump_manager, int _priority, std::function<bool()>&& _func, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_priority{_priority},
          m_func{std::move(_func)},
          m_taskset{std::move(_taskset)}
    {
    }

    Jump Jump::JumpManager::JumpManagerOperator::JumpIfCondition::operator()(std::nullptr_t) const& noexcept
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(m_func, JumpType::OneWay, nullptr);
            }
        }

        return {m_taskset, m_jump_manager};
    }
    Jump Jump::JumpManager::JumpManagerOperator::JumpIfCondition::operator()(std::nullptr_t) && noexcept
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(std::move(m_func), JumpType::OneWay, nullptr);
            }
        }

        return {std::move(m_taskset), std::move(m_jump_manager)};
    }


    Jump::JumpManager::JumpManagerOperator::JumpIfClass::JumpIfClass(const std::shared_ptr<JumpManager>& _jump_manager, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_taskset{_taskset}
    {
    }

    Jump::JumpManager::JumpManagerOperator::JumpIfClass Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](int _priority) const& noexcept
    {
        JumpIfClass tmp{*this};
        tmp.m_priority = _priority;
        return tmp;
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfClass Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](int _priority) && noexcept
    {
        JumpIfClass tmp{std::move(*this)};
        tmp.m_priority = _priority;
        return tmp;
    }

    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](const std::function<bool()>& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, _func, m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](std::function<bool()>&& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, std::move(_func), m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator()(const std::function<bool()>& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, _func, m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator()(std::function<bool()>&& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, std::move(_func), m_taskset};
    }

    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](const std::function<bool()>& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, _func, std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator[](std::function<bool()>&& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, std::move(_func), std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator()(const std::function<bool()>& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, _func, std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpIfCondition Jump::JumpManager::JumpManagerOperator::JumpIfClass::operator()(std::function<bool()>&& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, std::move(_func), std::move(m_taskset)};
    }


    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::JumpBackIfCondition(const std::shared_ptr<JumpManager>& _jump_manager, int _priority, const std::function<bool()>& _func, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_priority{_priority},
          m_func{_func},
          m_taskset{_taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::JumpBackIfCondition(const std::shared_ptr<JumpManager>& _jump_manager, int _priority, std::function<bool()>&& _func, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_priority{_priority},
          m_func{std::move(_func)},
          m_taskset{_taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::JumpBackIfCondition(std::shared_ptr<JumpManager>&& _jump_manager, int _priority, const std::function<bool()>& _func, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : m_jump_manager{std::move(_jump_manager)},
          m_priority{_priority},
          m_func{_func},
          m_taskset{std::move(_taskset)}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::JumpBackIfCondition(std::shared_ptr<JumpManager>&& _jump_manager, int _priority, std::function<bool()>&& _func, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : m_jump_manager{std::move(_jump_manager)},
          m_priority{_priority},
          m_func{std::move(_func)},
          m_taskset{std::move(_taskset)}
    {
    }

    Jump Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::operator()(std::nullptr_t) const& noexcept
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(m_func, JumpType::ReturnBack, nullptr);
            }
        }

        return {m_taskset, m_jump_manager};
    }
    Jump Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::operator()(std::nullptr_t) && noexcept
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(std::move(m_func), JumpType::ReturnBack, nullptr);
            }
        }

        return {std::move(m_taskset), std::move(m_jump_manager)};
    }


    Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::JumpBackIfClass(const std::shared_ptr<JumpManager>& _jump_manager, const std::shared_ptr<TaskSet>& _taskset) noexcept
        : m_jump_manager{_jump_manager},
          m_taskset{_taskset}
    {
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::JumpBackIfClass(std::shared_ptr<JumpManager>&& _jump_manager, std::shared_ptr<TaskSet>&& _taskset) noexcept
        : m_jump_manager{std::move(_jump_manager)},
          m_taskset{std::move(_taskset)}
    {
    }

    Jump::JumpManager::JumpManagerOperator::JumpBackIfClass Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](int _priority) const& noexcept
    {
        JumpBackIfClass tmp{*this};
        tmp.m_priority = _priority;
        return tmp;
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfClass Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](int _priority) && noexcept
    {
        JumpBackIfClass tmp{std::move(*this)};
        tmp.m_priority = _priority;
        return tmp;
    }

    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](const std::function<bool()>& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, _func, m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](std::function<bool()>&& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, std::move(_func), m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator()(const std::function<bool()>& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, _func, m_taskset};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator()(std::function<bool()>&& _func) const& noexcept
    {
        return {m_jump_manager, m_priority, std::move(_func), m_taskset};
    }

    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](const std::function<bool()>& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, _func, std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator[](std::function<bool()>&& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, std::move(_func), std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator()(const std::function<bool()>& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, _func, std::move(m_taskset)};
    }
    Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition Jump::JumpManager::JumpManagerOperator::JumpBackIfClass::operator()(std::function<bool()>&& _func) && noexcept
    {
        return {std::move(m_jump_manager), m_priority, std::move(_func), std::move(m_taskset)};
    }

}  //namespace Expr

}  //namespace TaskManager
