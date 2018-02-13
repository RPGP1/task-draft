#include "task_if.hpp"

namespace TaskManager
{

namespace Expr
{

    IfElse::IfElse(const condition_list_type& _cond_list)
        : m_condition_list{_cond_list},
          m_selected_task{nullptr}
    {
    }
    IfElse::IfElse(condition_list_type&& _cond_list) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
        : m_condition_list{std::move(_cond_list)},
          m_selected_task{nullptr}
    {
    }

    IfElse::IfElse(const IfElse& _other)
        : AbstTask{_other},
          m_condition_list{_other.m_condition_list},
          m_selected_task{nullptr}
    {
    }
    IfElse& IfElse::operator=(const IfElse& _other) &
    {
        AbstTask::operator=(_other);
        m_condition_list = _other.m_condition_list;
        m_selected_task = nullptr;
        return *this;
    }
    IfElse::IfElse(IfElse&& _other) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
        : AbstTask{std::move(_other)},
          m_condition_list{std::move(_other.m_condition_list)},
          m_selected_task{nullptr}
    {
    }
    IfElse& IfElse::operator=(IfElse&& _other) & noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
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


    std::shared_ptr<If> If::create(const condition_list_type& _cond_list)
    {
        std::shared_ptr<If> tmp{new If{_cond_list}};
        tmp->initialize_ptr(tmp);
        return tmp;
    }
    std::shared_ptr<If> If::create(condition_list_type&& _cond_list) noexcept(std::is_nothrow_constructible<If, condition_list_type&&>::value)
    {
        std::shared_ptr<If> tmp{new If{std::move(_cond_list)}};
        tmp->initialize_ptr(tmp);
        return tmp;
    }

    If::If(const condition_list_type& _cond_list)
        : IfElse{_cond_list},
          ElseIf{},
          m_this_ptr{}
    {
    }
    If::If(condition_list_type&& _cond_list) noexcept(std::is_nothrow_constructible<IfElse, condition_list_type&&>::value)
        : IfElse{std::move(_cond_list)},
          ElseIf{},
          m_this_ptr{}
    {
    }

    void If::initialize_ptr(const std::shared_ptr<If>& _ptr) noexcept
    {
        m_this_ptr = _ptr;
        ElseIf.set_parent(_ptr);
    }

    If& If::operator=(const If& _other) &
    {
        IfElse::operator=(_other);
        return *this;
    }
    If& If::operator=(If&& _other) & noexcept(std::is_nothrow_move_assignable<IfElse>::value)
    {
        IfElse::operator=(std::move(_other));
        return *this;
    }

    std::shared_ptr<If> If::copy()
    {
        return create(m_condition_list);
    }

    void If::add_condition(const condition_list_type::value_type& _pair)
    {
        // 条件が関数呼び出し可能
        if (_pair.first) {
            m_condition_list.push_back(_pair);
        }
    }
    void If::add_condition(condition_list_type::value_type&& _pair) noexcept(std::is_nothrow_move_constructible<TaskSet>::value)
    {
        // 条件が関数呼び出し可能
        if (_pair.first) {
            m_condition_list.push_back(std::move(_pair));
        }
    }


    If::ElseIfClass::ElseIfClass() noexcept
        : m_parent_ptr{}
    {
    }

    void If::ElseIfClass::set_parent(const std::shared_ptr<If>& _parent) noexcept
    {
        m_parent_ptr = _parent;
    }

    If::ElseIfClass::ElseIfCondition If::ElseIfClass::operator[](const std::function<bool()>& _func) const noexcept
    {
        return ElseIfCondition{m_parent_ptr.lock(), _func};
    }
    If::ElseIfClass::ElseIfCondition If::ElseIfClass::operator[](std::function<bool()>&& _func) const noexcept
    {
        return ElseIfCondition{m_parent_ptr.lock(), std::move(_func)};
    }

    If::ElseIfClass::ElseIfCondition If::ElseIfClass::operator()(const std::function<bool()>& _func) const noexcept
    {
        return ElseIfCondition{m_parent_ptr.lock(), _func};
    }
    If::ElseIfClass::ElseIfCondition If::ElseIfClass::operator()(std::function<bool()>&& _func) const noexcept
    {
        return ElseIfCondition{m_parent_ptr.lock(), std::move(_func)};
    }


    If::ElseIfClass::ElseIfCondition::ElseIfCondition(std::shared_ptr<If>&& _if_task, const std::function<bool()>& _func) noexcept
        : m_if_task{std::move(_if_task)},
          m_condition{_func}
    {
    }
    If::ElseIfClass::ElseIfCondition::ElseIfCondition(std::shared_ptr<If>&& _if_task, std::function<bool()>&& _func) noexcept
        : m_if_task{std::move(_if_task)},
          m_condition{std::move(_func)}
    {
    }

    If::ElseIfClass::ElseIfCondition::ElseIfCondition(const ElseIfCondition& _other) noexcept
        : m_if_task{_other.m_if_task},
          m_condition{_other.m_condition}
    {
    }
    If::ElseIfClass::ElseIfCondition& If::ElseIfClass::ElseIfCondition::operator=(const ElseIfCondition& _other) & noexcept
    {
        m_if_task = _other.m_if_task;
        m_condition = _other.m_condition;
        return *this;
    }
    If::ElseIfClass::ElseIfCondition::ElseIfCondition(ElseIfCondition&& _other) noexcept
        : m_if_task{std::move(_other.m_if_task)},
          m_condition{std::move(_other.m_condition)}
    {
        _other.m_if_task = nullptr;
        _other.m_condition = nullptr;
    }
    If::ElseIfClass::ElseIfCondition& If::ElseIfClass::ElseIfCondition::operator=(ElseIfCondition&& _other) & noexcept
    {
        m_if_task = std::move(_other.m_if_task);
        _other.m_if_task = nullptr;
        m_condition = _other.m_condition;
        _other.m_condition = nullptr;
        return *this;
    }


    IfCondition::IfCondition(const std::function<bool()>& _func) noexcept
        : m_condition{_func}
    {
    }
    IfCondition::IfCondition(std::function<bool()>&& _func) noexcept
        : m_condition{std::move(_func)}
    {
    }

    IfCondition::IfCondition(const IfCondition& _other) noexcept
        : m_condition{_other.m_condition}
    {
    }
    IfCondition& IfCondition::operator=(const IfCondition& _other) & noexcept
    {
        m_condition = _other.m_condition;
        return *this;
    }
    IfCondition::IfCondition(IfCondition&& _other) noexcept
        : m_condition{std::move(_other.m_condition)}
    {
        _other.m_condition = nullptr;
    }
    IfCondition& IfCondition::operator=(IfCondition&& _other) & noexcept
    {
        m_condition = std::move(_other.m_condition);
        _other.m_condition = nullptr;
        return *this;
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
