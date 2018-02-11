#pragma once

#include "./abst_task.hpp"
#include "./task_set.hpp"

namespace TaskManager
{

namespace Expr
{

    class While : public AbstTask
    {
    protected:
        std::function<bool()> m_condition;
        TaskSet m_taskset;

    private:
        bool m_should_eval;  //!< 実行中かを示すフラグ

    public:
        While(const std::function<bool()>&, const TaskSet&);
        While(const std::function<bool()>&, TaskSet&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);
        While(std::function<bool()>&&, const TaskSet&);
        While(std::function<bool()>&&, TaskSet&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);

        virtual ~While() noexcept {}

        While(const While&);
        While& operator=(const While&) &;
        While(While&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);
        While& operator=(While&&) & noexcept(std::is_nothrow_move_assignable<TaskSet>::value);

    protected:
        void init() override;
        bool eval() override;

        void interrupt() override;
    };


    class WhileCondition
    {
    private:
        std::function<bool()> m_condition;

    public:
        WhileCondition(const std::function<bool()>&) noexcept;
        WhileCondition(std::function<bool()>&&) noexcept;

        virtual ~WhileCondition() noexcept {}

        WhileCondition(const WhileCondition&) noexcept;
        WhileCondition& operator=(const WhileCondition&) & noexcept;
        WhileCondition(WhileCondition&&) noexcept;
        WhileCondition& operator=(WhileCondition&&) & noexcept;

        template <typename... TaskClasses>
        While operator()(TaskClasses&&...) const&;
        template <typename... TaskClasses>
        While operator()(TaskClasses&&...) &&;
    };


    struct WhileOperator {
        WhileCondition operator[](const std::function<bool()>&) const& noexcept;
        WhileCondition operator[](std::function<bool()>&&) const& noexcept;
        WhileCondition operator()(const std::function<bool()>&) const& noexcept;
        WhileCondition operator()(std::function<bool()>&&) const& noexcept;
    };


    struct UntilOperator {
        WhileCondition operator[](const std::function<bool()>&) const& noexcept;
        WhileCondition operator[](std::function<bool()>&&) const& noexcept;
        WhileCondition operator()(const std::function<bool()>&) const& noexcept;
        WhileCondition operator()(std::function<bool()>&&) const& noexcept;
    };


    template <typename... TaskClasses>
    While WhileCondition::operator()(TaskClasses&&... tasks) const&
    {
        if (m_condition) {
            return While{m_condition, TaskSet{std::forward<TaskClasses>(tasks)...}};
        }
        return While{nullptr, {}};
    }
    template <typename... TaskClasses>
    While WhileCondition::operator()(TaskClasses&&... tasks) &&
    {
        if (m_condition) {
            While tmp{std::move(m_condition), TaskSet{std::forward<TaskClasses>(tasks)...}};
            m_condition = nullptr;
            return tmp;
        }
        return While{nullptr, {}};
    }


    struct WaitOperator {
        While operator[](const std::function<bool()>& _func) const noexcept
        {
            if (_func) {
                return While{[_func]() mutable { return !_func(); }, {}};
            }
            return While{nullptr, {}};
        }
        While operator[](std::function<bool()>&& _func) const noexcept
        {
            if (_func) {
                // clang-format off
                return While{[_func = std::move(_func)]() mutable { return !_func(); }, {}};
            }
            return While{nullptr, {}};
        }

        While operator()(const std::function<bool()>& _func) const noexcept
        {
            if (_func) {
                return While{[_func]() mutable { return !_func(); }, {}};
            }
            return While{nullptr, {}};
        }
        While operator()(std::function<bool()>&& _func) const noexcept
        {
            if (_func) {
                // clang-format off
                return While{[_func = std::move(_func)]() mutable { return !_func(); }, {}};
            }
            return While{nullptr, {}};
        }
    };
// clang-format on


}  // namespace Expr

constexpr Expr::WhileOperator While;
constexpr Expr::UntilOperator Until;
constexpr Expr::WaitOperator Wait;

}  // namespace TaskManager
