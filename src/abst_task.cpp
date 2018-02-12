#include "abst_task.hpp"

#include <exception>
#include <iostream>

// TODO: AbstTask::force_quitでcerr使用中

namespace TaskManager
{

namespace Expr
{

    NextTask::NextTask(NextTask&& _other) noexcept
        : m_finish{_other.m_finish},
          m_next{_other.m_next}
    {
        _other.m_next = nullptr;
    }
    NextTask& NextTask::operator=(NextTask&& _other) & noexcept
    {
        m_finish = _other.m_finish;
        m_next = _other.m_next;
        _other.m_next = nullptr;
        return *this;
    }


    AbstTask::AbstTask() noexcept
        : m_me_on_eval{false},
          m_task_on_eval{nullptr},
          m_running{false},
          interrupt_func{nullptr}
    {
    }
    AbstTask::~AbstTask() noexcept
    {
        force_quit();
    }

    AbstTask::AbstTask(const AbstTask& _other) noexcept
        : m_me_on_eval{false},
          m_task_on_eval{nullptr},
          m_running{false},
          interrupt_func{_other.interrupt_func}
    {
    }
    AbstTask& AbstTask::operator=(const AbstTask& _other) & noexcept
    {
        force_quit_task();
        interrupt_func = _other.interrupt_func;
        return *this;
    }
    AbstTask::AbstTask(AbstTask&& _other) noexcept
        : m_me_on_eval{false},
          m_task_on_eval{nullptr},
          m_running{false},
          interrupt_func{nullptr}
    {
        _other.force_quit_task();
        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
    }
    AbstTask& AbstTask::operator=(AbstTask&& _other) & noexcept
    {
        force_quit_task();
        _other.force_quit_task();
        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
        return *this;
    }

    bool AbstTask::evaluate()
    {
        auto result = (m_task_on_eval ? m_task_on_eval.get() : this)->evaluate_task();

        if (auto next = result.pointer()) {  // 次のタスクを指定
            m_task_on_eval = next;
            return false;

        } else if (!result) {  // 引き続き実行
            return false;

        } else {  // 終了
            m_task_on_eval = nullptr;
            return true;
        }
    }

    NextTask AbstTask::evaluate_task()
    {
        std::lock_guard<std::mutex> lock{m_eval_mutex};

        if (!m_me_on_eval) {
            init();
            m_me_on_eval = true;
        }

        auto result = eval();

        if (result) {
            m_me_on_eval = false;
            quit();
            return result;
        }

        return false;
    }

    void AbstTask::force_quit() noexcept
    {
        (m_task_on_eval ? m_task_on_eval.get() : this)->force_quit_task();
        m_task_on_eval = nullptr;
    }

    // 各種コンストラクタで呼ばれる可能性が有るため、
    // 全タスククラスに影響が有ると考えると例外を出させたくない
    void AbstTask::force_quit_task() noexcept
    {
        std::lock_guard<std::mutex> lock{m_eval_mutex};

        if (m_me_on_eval) {
            m_me_on_eval = false;
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
                std::cerr << "error occurred during a task's force_quit...\n"
                          << e.what() << std::endl;

            } catch (...) {
                std::cerr << "error occurred during a task's force_quit()...\n(not an instance of std::exception 's derived class)" << std::endl;
            }
#endif
        }
    }

    void AbstTask::start() noexcept
    {
        if (m_running) {
            std::cerr << "start() a task which has already started" << std::endl;
            return;
        }

        m_running = true;
    }
    void AbstTask::resume()
    {
        if (m_running) {
            auto result = evaluate();

            if (result) {
                m_running = false;
            }
        }
    }
    void AbstTask::stop() noexcept
    {
        if (!m_running) {
            std::cerr << "stop() a task which has already stopped" << std::endl;
            return;
        }

        m_running = false;
    }
    void AbstTask::reset() noexcept
    {
        force_quit();
    }

    bool AbstTask::running() noexcept
    {
        return m_running;
    }

}  // namespace Expr

}  // namespace TaskManager
