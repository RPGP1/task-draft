#include "abst_task.hpp"

#include <exception>
#include <iostream>

// TODO: AbstTask::force_quitでcerr使用中

namespace TaskManager
{

namespace Expr
{

    AbstTask::AbstTask() noexcept
        : m_on_eval{false},
          interrupt_func{nullptr}
    {
    }

    AbstTask::AbstTask(const AbstTask& _other) noexcept
        : m_on_eval{false},
          interrupt_func{_other.interrupt_func}
    {
    }
    AbstTask& AbstTask::operator=(const AbstTask& _other) & noexcept
    {
        force_quit();
        interrupt_func = _other.interrupt_func;
        return *this;
    }
    AbstTask::AbstTask(AbstTask&& _other) noexcept
        : m_on_eval{false},
          interrupt_func{nullptr}
    {
        _other.force_quit();
        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
    }
    AbstTask& AbstTask::operator=(AbstTask&& _other) & noexcept
    {
        force_quit();
        _other.force_quit();
        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
        return *this;
    }

    bool AbstTask::evaluate()
    {
        if (!m_on_eval) {
            init();
            m_on_eval = true;
        }

        if (eval()) {
            m_on_eval = false;
            quit();
            return true;
        }

        return false;
    }

    // 各種コンストラクタで呼ばれる可能性が有るため、
    // 全タスククラスに影響が有ると考えると例外を出させたくない
    void AbstTask::force_quit() noexcept
    {
        if (m_on_eval) {
            m_on_eval = false;
// try-catch only in Release build
#ifdef NDEBUG
            try {
#endif
                interrupt();
                if (interrupt_func) {
                    interrupt_func();
                }

#ifdef NDEBUG
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;

            } catch (...) {
            }
#endif
        }
    }

}  // namespace Expr

}  // namespace TaskManager
