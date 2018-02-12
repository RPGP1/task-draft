#include "task_do.hpp"

namespace TaskManager
{

namespace Expr
{

    DoWhile::DoWhile(const TaskSet& _task, const std::function<bool()>& _func)
        : While{_func, _task}
    {
    }
    DoWhile::DoWhile(const TaskSet& _task, std::function<bool()>&& _func)
        : While{std::move(_func), _task}
    {
    }
    DoWhile::DoWhile(TaskSet&& _task, const std::function<bool()>& _func) noexcept(std::is_nothrow_constructible<While, const std::function<bool()>&, TaskSet&&>::value)
        : While{_func, std::move(_task)}
    {
    }
    DoWhile::DoWhile(TaskSet&& _task, std::function<bool()>&& _func) noexcept(std::is_nothrow_constructible<While, std::function<bool()>&&, TaskSet&&>::value)
        : While{std::move(_func), std::move(_task)}
    {
    }

    DoWhile::DoWhile(const DoWhile& _other)
        : While{_other}
    {
    }
    DoWhile& DoWhile::operator=(const DoWhile& _other) &
    {
        While::operator=(_other);
        return *this;
    }
    DoWhile::DoWhile(DoWhile&& _other) noexcept(std::is_nothrow_move_constructible<While>::value)
        : While{std::move(_other)}
    {
    }
    DoWhile& DoWhile::operator=(DoWhile&& _other) & noexcept(std::is_nothrow_move_assignable<While>::value)
    {
        While::operator=(std::move(_other));
        return *this;
    }

    NextTask DoWhile::eval()
    {
        return m_taskset.evaluate() && !(m_condition && m_condition());
    }


    DoTaskSet::DoTaskSet(const std::shared_ptr<TaskSet>& _task) noexcept
        : While{_task},
          Until{_task}
    {
    }
    DoTaskSet::DoTaskSet(std::shared_ptr<TaskSet>&& _task) noexcept
        : While{_task},
          Until{std::move(_task)}
    {
    }

    DoTaskSet::DoTaskSet(const DoTaskSet& _other) noexcept
        : While{_other.While},
          Until{_other.Until}
    {
    }
    DoTaskSet& DoTaskSet::operator=(const DoTaskSet& _other) & noexcept
    {
        While = _other.While;
        Until = _other.Until;
        return *this;
    }
    DoTaskSet::DoTaskSet(DoTaskSet&& _other) noexcept
        : While{std::move(_other.While)},
          Until{std::move(_other.Until)}
    {
    }
    DoTaskSet& DoTaskSet::operator=(DoTaskSet&& _other) & noexcept
    {
        While = std::move(_other.While);
        Until = std::move(_other.Until);
        return *this;
    }


    DoTaskSet::WhileClass::WhileClass(const std::shared_ptr<TaskSet>& _task) noexcept
        : m_taskset{_task}
    {
    }
    DoTaskSet::WhileClass::WhileClass(std::shared_ptr<TaskSet>&& _task) noexcept
        : m_taskset{std::move(_task)}
    {
    }

    DoTaskSet::WhileClass::WhileClass(const WhileClass& _other) noexcept
        : m_taskset{_other.m_taskset}
    {
    }
    DoTaskSet::WhileClass& DoTaskSet::WhileClass::operator=(const WhileClass& _other) & noexcept
    {
        m_taskset = _other.m_taskset;
        return *this;
    }
    DoTaskSet::WhileClass::WhileClass(WhileClass&& _other) noexcept
        : m_taskset{std::move(_other.m_taskset)}
    {
        _other.m_taskset = nullptr;
    }
    DoTaskSet::WhileClass& DoTaskSet::WhileClass::operator=(WhileClass&& _other) & noexcept
    {
        m_taskset = std::move(_other.m_taskset);
        _other.m_taskset = nullptr;
        return *this;
    }

    DoWhile DoTaskSet::WhileClass::operator[](const std::function<bool()>& _func) const
    {
        return DoWhile(*m_taskset, _func);
    }
    DoWhile DoTaskSet::WhileClass::operator[](std::function<bool()>&& _func) const
    {
        return DoWhile(*m_taskset, std::move(_func));
    }
    DoWhile DoTaskSet::WhileClass::operator()(const std::function<bool()>& _func) const
    {
        return DoWhile(*m_taskset, _func);
    }
    DoWhile DoTaskSet::WhileClass::operator()(std::function<bool()>&& _func) const
    {
        return DoWhile(*m_taskset, std::move(_func));
    }


    DoTaskSet::UntilClass::UntilClass(const std::shared_ptr<TaskSet>& _task) noexcept
        : WhileClass{_task}
    {
    }
    DoTaskSet::UntilClass::UntilClass(std::shared_ptr<TaskSet>&& _task) noexcept
        : WhileClass{std::move(_task)}
    {
    }

    DoTaskSet::UntilClass::UntilClass(const UntilClass& _other) noexcept
        : WhileClass(_other)
    {
    }
    DoTaskSet::UntilClass& DoTaskSet::UntilClass::operator=(const UntilClass& _other) & noexcept
    {
        WhileClass::operator=(_other);
        return *this;
    }
    DoTaskSet::UntilClass::UntilClass(UntilClass&& _other) noexcept
        : WhileClass(std::move(_other))
    {
    }
    DoTaskSet::UntilClass& DoTaskSet::UntilClass::operator=(UntilClass&& _other) & noexcept
    {
        WhileClass::operator=(std::move(_other));
        return *this;
    }

    // clang-format off
    DoWhile DoTaskSet::UntilClass::operator[](const std::function<bool()>& _func) const
    {
        if (_func) {
            return WhileClass::operator[]([_func]() mutable { return !_func(); });
        }
        return WhileClass::operator[](nullptr);
    }
    DoWhile DoTaskSet::UntilClass::operator[](std::function<bool()>&& _func) const
    {
        if (_func) {
            return WhileClass::operator[]([_func = std::move(_func)]() mutable { return !_func(); });
        }
        return WhileClass::operator[](nullptr);
    }
    DoWhile DoTaskSet::UntilClass::operator()(const std::function<bool()>& _func) const
    {
        if (_func) {
            return WhileClass::operator()([_func]() mutable { return !_func(); });
        }
        return WhileClass::operator()(nullptr);
    }
    DoWhile DoTaskSet::UntilClass::operator()(std::function<bool()>&& _func) const
    {
        if (_func) {
            return WhileClass::operator()([_func = std::move(_func)]() mutable { return !_func(); });
        }
        return WhileClass::operator()(nullptr);
    }
    // clang-format on

}  // namespace Expr

}  // namespace TaskManager
