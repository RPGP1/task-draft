#include "task_if.hpp"

namespace TaskManager
{

namespace Expr
{

    IfElse::IfElse(const condition_list_type& _cond_list)
        : m_condition_list{_cond_list}
    {
    }
    IfElse::IfElse(condition_list_type&& _cond_list) noexcept
        : m_condition_list{std::move(_cond_list)}
    {
    }

    IfElse::IfElse(const IfElse& _other)
        : AbstTask{_other},
          m_condition_list{_other.m_condition_list}
    {
    }
    IfElse& IfElse::operator=(const IfElse& _other) &
    {
        AbstTask::operator=(_other);
        m_condition_list = _other.m_condition_list;
        m_selected_task = nullptr;
        return *this;
    }
    IfElse::IfElse(IfElse&& _other) noexcept
        : AbstTask{std::move(_other)},
          m_condition_list{std::move(_other.m_condition_list)}
    {
    }
    IfElse& IfElse::operator=(IfElse&& _other) & noexcept
    {
        AbstTask::operator=(std::move(_other));
        m_condition_list = std::move(_other.m_condition_list);
        m_selected_task = nullptr;
        return *this;
    }

    void IfElse::init()
    {
        m_selected_task = nullptr;

        for (auto& cond_pair : m_condition_list) {
            // 条件が真を示したら
            if (cond_pair.first && cond_pair.first()) {
                // ポインタを持ちながらも所有権は無い
                m_selected_task = std::shared_ptr<TaskSet>{std::shared_ptr<TaskSet>{nullptr}, &cond_pair.second};
                break;
            }
        }
    }

    NextTask IfElse::eval()
    {
        // 条件が真となったものが有ったら
        if (m_selected_task) {
            return evaluate(*m_selected_task);
        }

        return true;
    }

    void IfElse::interrupt()
    {
        if (m_selected_task) {
            force_quit(*m_selected_task);
        }
        quit();
    }


    If& If::operator=(const If& _other) &
    {
        IfElse::operator=(_other);
        return *this;
    }
    If& If::operator=(If&& _other) & noexcept
    {
        IfElse::operator=(std::move(_other));
        return *this;
    }

    std::shared_ptr<If::IfFunction> If::operator->() const&
    {
        return std::make_shared<IfFunction>(m_condition_list);
    }
    std::shared_ptr<If::IfFunction> If::operator->() && noexcept
    {
        return std::make_shared<IfFunction>(std::move(m_condition_list));
    }


    If::IfFunction::IfFunction(const condition_list_type& _cond_list)
        : m_cond_list_ptr{std::make_shared<condition_list_type>(_cond_list)},
          ElseIf{m_cond_list_ptr}
    {
    }
    If::IfFunction::IfFunction(condition_list_type&& _cond_list) noexcept
        : m_cond_list_ptr{std::make_shared<condition_list_type>(std::move(_cond_list))},
          ElseIf{m_cond_list_ptr}
    {
    }


    If::IfFunction::ElseIfClass::ElseIfCondition If::IfFunction::ElseIfClass::operator[](const std::function<bool()>& _func) const noexcept
    {
        return ElseIfCondition{*m_cond_list_ptr, _func};
    }
    If::IfFunction::ElseIfClass::ElseIfCondition If::IfFunction::ElseIfClass::operator[](std::function<bool()>&& _func) const noexcept
    {
        return ElseIfCondition{*m_cond_list_ptr, std::move(_func)};
    }

    If::IfFunction::ElseIfClass::ElseIfCondition If::IfFunction::ElseIfClass::operator()(const std::function<bool()>& _func) const noexcept
    {
        return ElseIfCondition{*m_cond_list_ptr, _func};
    }
    If::IfFunction::ElseIfClass::ElseIfCondition If::IfFunction::ElseIfClass::operator()(std::function<bool()>&& _func) const noexcept
    {
        return ElseIfCondition{*m_cond_list_ptr, std::move(_func)};
    }


    If::IfFunction::ElseIfClass::ElseIfCondition::ElseIfCondition(const condition_list_type& _cond_list, const std::function<bool()>& _func) noexcept
        : m_condition_list{_cond_list},
          m_condition{_func}
    {
    }
    If::IfFunction::ElseIfClass::ElseIfCondition::ElseIfCondition(const condition_list_type& _cond_list, std::function<bool()>&& _func) noexcept
        : m_condition_list{_cond_list},
          m_condition{std::move(_func)}
    {
    }


    IfCondition::IfCondition(const std::function<bool()>& _func) noexcept
        : m_condition{_func}
    {
    }
    IfCondition::IfCondition(std::function<bool()>&& _func) noexcept
        : m_condition{std::move(_func)}
    {
    }


    IfCondition IfOperator::operator[](const std::function<bool()>& _func) const& noexcept
    {
        return IfCondition{_func};
    }
    IfCondition IfOperator::operator[](std::function<bool()>&& _func) const& noexcept
    {
        return IfCondition{std::move(_func)};
    }
    IfCondition IfOperator::operator()(const std::function<bool()>& _func) const& noexcept
    {
        return IfCondition{_func};
    }
    IfCondition IfOperator::operator()(std::function<bool()>&& _func) const& noexcept
    {
        return IfCondition{std::move(_func)};
    }

}  // namespace Expr

}  // namespace TaskManager
