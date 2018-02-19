#include "task_jump.hpp"


namespace TaskManager
{

namespace Expr
{
    Jump::Jump(const std::shared_ptr<TaskSet>& _taskset, const std::shared_ptr<JumpManager>& _jump_manager) noexcept
        : m_taskset{_taskset},
          m_jump_manager{_jump_manager}
    {
    }
    Jump::Jump(std::shared_ptr<TaskSet>&& _taskset, std::shared_ptr<JumpManager>&& _jump_manager) noexcept
        : m_taskset{std::move(_taskset)},
          m_jump_manager{std::move(_jump_manager)}
    {
    }

    Jump::Jump(const Jump& _other)
        : AbstTask{_other},
          m_taskset{std::make_shared<TaskSet>(*_other.m_taskset)},
          m_jump_manager{std::make_shared<JumpManager>(*_other.m_jump_manager)}
    {
    }
    Jump& Jump::operator=(const Jump& _other) &
    {
        AbstTask::operator=(_other);
        m_taskset = std::make_shared<TaskSet>(*_other.m_taskset);
        m_jump_manager = std::make_shared<JumpManager>(*_other.m_jump_manager);
        return *this;
    }
    Jump::Jump(Jump&& _other) noexcept
        : AbstTask{std::move(_other)},
          m_taskset{std::move(_other.m_taskset)},
          m_jump_manager{std::move(_other.m_jump_manager)}
    {
        _other.m_taskset = nullptr;
        _other.m_jump_manager = nullptr;
    }
    Jump& Jump::operator=(Jump&& _other) & noexcept
    {
        AbstTask::operator=(std::move(_other));
        m_taskset = std::move(_other.m_taskset);
        _other.m_taskset = nullptr;
        m_jump_manager = std::move(_other.m_jump_manager);
        _other.m_jump_manager = nullptr;
        return *this;
    }

    NextTask Jump::eval()
    {
        auto result = true;

        if (m_taskset) {
            result = evaluate(*m_taskset);
        }

        if (!m_jump_manager) {
            return result;
        }

        if (auto jump = m_jump_manager->check())  //ジャンプ条件判定が真を返した
        {
            auto& jump_target = jump.value();

            if (static_cast<bool>(jump_target.type)) {  //ReturnBack
                if (set_jump(jump_target.priority, nullptr) && jump_target.task_ptr) {
                    force_quit(*m_taskset);
                    return {jump_target.task_ptr};
                }

            } else {  //OneWay
                if (set_jump(jump_target.priority, jump_target.task_ptr) && jump_target.task_ptr) {
                    return false;
                }
            }
        }

        return result;
    }
    void Jump::interrupt()
    {
        if (m_taskset) {
            force_quit(*m_taskset);
        }
        quit();
    }

    std::shared_ptr<Jump::JumpManager::JumpManagerOperator> Jump::operator->() const& noexcept
    {
        return std::make_shared<JumpManager::JumpManagerOperator>(m_jump_manager, m_taskset);
    }
    std::shared_ptr<Jump::JumpManager::JumpManagerOperator> Jump::operator->() && noexcept
    {
        return std::make_shared<JumpManager::JumpManagerOperator>(std::move(m_jump_manager), std::move(m_taskset));
    }


    Jump::JumpManager::JumpManager(const JumpManager& _other)
        : m_jump_list{std::make_shared<jump_cond_list_t>(*_other.m_jump_list)}
    {
    }
    Jump::JumpManager& Jump::JumpManager::operator=(const JumpManager& _other) &
    {
        m_jump_list = std::make_shared<jump_cond_list_t>(*_other.m_jump_list);
        return *this;
    }

    std::optional<Jump::JumpManager::JumpTarget> Jump::JumpManager::check()
    {
        if (m_jump_list) {

            //priorityが高い順に取り出し
            for (auto i = m_jump_list->rbegin(); i != m_jump_list->rend(); ++i) {

                //同priorityでは先に登録した順に取り出し
                for (auto&& cond : i->second) {
                    if (auto& func = std::get<std::function<bool()>>(cond)) {
                        if (func()) {  //条件成立

                            return JumpTarget{i->first,
                                std::get<JumpType>(cond),
                                std::make_shared<TaskSet>(*std::get<std::shared_ptr<TaskSet>>(cond))};
                        }
                    }
                }
            }
        }

        return std::nullopt;
    }


    Jump::EmbeddedJump::EmbeddedJump(const Jump& _jump)
        : m_taskset{_jump.m_taskset ? *_jump.m_taskset : TaskSet{}},
          m_jump_manager{_jump.m_jump_manager}
    {
    }
    Jump::EmbeddedJump::EmbeddedJump(Jump&& _jump) noexcept
        : m_taskset{_jump.m_taskset ? *_jump.m_taskset : TaskSet{}},
          m_jump_manager{std::move(_jump.m_jump_manager)}
    {
        _jump.m_jump_manager = nullptr;
    }

    Jump::EmbeddedJump::EmbeddedJump(const EmbeddedJump& _other)
        : AbstTask{_other}, m_taskset{_other.m_taskset},
          m_jump_manager{_other.m_jump_manager}
    {
    }
    Jump::EmbeddedJump& Jump::EmbeddedJump::operator=(const EmbeddedJump& _other) &
    {
        AbstTask::operator=(_other);
        m_taskset = _other.m_taskset;
        m_jump_manager = _other.m_jump_manager;
        return *this;
    }
    Jump::EmbeddedJump::EmbeddedJump(EmbeddedJump&& _other) noexcept
        : AbstTask{_other},
          m_taskset{std::move(_other.m_taskset)},
          m_jump_manager{std::move(_other.m_jump_manager)}
    {
        _other.m_jump_manager = nullptr;
    }
    Jump::EmbeddedJump& Jump::EmbeddedJump::operator=(EmbeddedJump&& _other) & noexcept
    {
        AbstTask::operator=(std::move(_other));
        m_taskset = std::move(_other.m_taskset);
        m_jump_manager = std::move(_other.m_jump_manager);
        _other.m_jump_manager = nullptr;
        return *this;
    }

    NextTask Jump::EmbeddedJump::eval()
    {
        auto result = evaluate(m_taskset);

        if (!m_jump_manager) {
            return result;
        }

        if (auto jump = m_jump_manager->check())  //ジャンプ条件判定が真を返した
        {
            auto& jump_target = jump.value();

            if (static_cast<bool>(jump_target.type)) {  //ReturnBack
                if (set_jump(jump_target.priority, nullptr) && jump_target.task_ptr) {
                    force_quit(m_taskset);
                    return {jump_target.task_ptr};
                }

            } else {  //OneWay
                if (set_jump(jump_target.priority, jump_target.task_ptr) && jump_target.task_ptr) {
                    return false;
                }
            }
        }

        return result;
    }
    void Jump::EmbeddedJump::interrupt()
    {
        force_quit(m_taskset);
        quit();
    }

}  //namespace Expr

}  //namespace TaskManager
