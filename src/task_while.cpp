#include "task_while.hpp"

namespace TaskManager
{

namespace Expr
{

    While::While(const std::function<bool()>& _func, const TaskSet& _task)
        : m_condition{_func},
          m_taskset{_task},
          m_should_eval{false}
    {
    }
    While::While(const std::function<bool()>& _func, TaskSet&& _task) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
        : m_condition{_func},
          m_taskset{std::move(_task)},
          m_should_eval{false}
    {
    }
    While::While(std::function<bool()>&& _func, const TaskSet& _task)
        : m_condition{std::move(_func)},
          m_taskset{_task},
          m_should_eval{false}
    {
    }
    While::While(std::function<bool()>&& _func, TaskSet&& _task) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
        : m_condition{std::move(_func)},
          m_taskset{std::move(_task)},
          m_should_eval{false}
    {
    }

    While::While(const While& _other)
        : AbstTask{_other},
          m_condition{_other.m_condition},
          m_taskset{_other.m_taskset},
          m_should_eval{false}
    {
    }
    While& While::operator=(const While& _other) &
    {
        AbstTask::operator=(_other);
        m_condition = _other.m_condition;
        m_taskset = _other.m_taskset;
        m_should_eval = false;
        return *this;
    }
    While::While(While&& _other) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
        : AbstTask{std::move(_other)},
          m_condition{std::move(_other.m_condition)},
          m_taskset{std::move(_other.m_taskset)},
          m_should_eval{false}
    {
        _other.m_condition = nullptr;
    }
    While& While::operator=(While&& _other) & noexcept(std::is_nothrow_move_assignable<TaskSet>::value)
    {
        AbstTask::operator=(std::move(_other));
        m_condition = std::move(_other.m_condition);
        _other.m_condition = nullptr;
        m_taskset = std::move(_other.m_taskset);
        m_should_eval = false;
        return *this;
    }

    void While::init()
    {
        // 実行するべきか
        m_should_eval = m_condition && m_condition();
        // なお、条件式が生きていれば一度は実行されることが保証される
    }

    NextTask While::eval()
    {
        /*
        if (m_should_eval) {  // initで判定した、実行するかどうか
            if (m_taskset->evaluate()) {  // タスクセットの実行終了
                return !m_condition();  // もう一周するならreturn false
            }else{
                return false;  // タスクが未完了
            }
        }

        return true;
        */

        // 上と同値
        return !m_should_eval || (evaluate(m_taskset) && !m_condition());
    }

    void While::interrupt()
    {
        force_quit(m_taskset);
        quit();
    }


    WhileCondition::WhileCondition(const std::function<bool()>& _func) noexcept
        : m_condition{_func}
    {
    }
    WhileCondition::WhileCondition(std::function<bool()>&& _func) noexcept
        : m_condition{std::move(_func)}
    {
    }

    WhileCondition::WhileCondition(const WhileCondition& _other) noexcept
        : m_condition{_other.m_condition}
    {
    }
    WhileCondition& WhileCondition::operator=(const WhileCondition& _other) & noexcept
    {
        m_condition = _other.m_condition;
        return *this;
    }
    WhileCondition::WhileCondition(WhileCondition&& _other) noexcept
        : m_condition{std::move(_other.m_condition)}
    {
        _other.m_condition = nullptr;
    }
    WhileCondition& WhileCondition::operator=(WhileCondition&& _other) & noexcept
    {
        m_condition = std::move(_other.m_condition);
        _other.m_condition = nullptr;
        return *this;
    }


    WhileCondition WhileOperator::operator[](const std::function<bool()>& _func) const& noexcept
    {
        return WhileCondition{_func};
    }
    WhileCondition WhileOperator::operator[](std::function<bool()>&& _func) const& noexcept
    {
        return WhileCondition{std::move(_func)};
    }
    WhileCondition WhileOperator::operator()(const std::function<bool()>& _func) const& noexcept
    {
        return WhileCondition{_func};
    }
    WhileCondition WhileOperator::operator()(std::function<bool()>&& _func) const& noexcept
    {
        return WhileCondition{std::move(_func)};
    }


    // clang-format off
    WhileCondition UntilOperator::operator[](const std::function<bool()>& _func) const& noexcept
    {
        if (_func) {
            return WhileCondition{[_func]() mutable { return !_func(); }};
        }
        return WhileCondition{nullptr};
    }
    WhileCondition UntilOperator::operator[](std::function<bool()>&& _func) const& noexcept
    {
        if (_func) {
            return WhileCondition{[_func = std::move(_func)]() mutable { return !_func(); }};
        }
        return WhileCondition{nullptr};
    }
    WhileCondition UntilOperator::operator()(const std::function<bool()>& _func) const& noexcept
    {
        if (_func) {
            return WhileCondition{[_func]() mutable { return !_func(); }};
        }
        return WhileCondition{nullptr};
    }
    WhileCondition UntilOperator::operator()(std::function<bool()>&& _func) const& noexcept
    {
        if (_func) {
            return WhileCondition{[_func = std::move(_func)]() mutable { return !_func(); }};
        }
        return WhileCondition{nullptr};
    }
// clang-format on


}  // namespace Expr

}  // namespace TaskManager
