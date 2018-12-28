#include "abst_task.hpp"

#include <exception>
#include <iostream>

// TODO: AbstTask::force_quitでcerr使用中

namespace TaskManager
{

namespace Expr
{


    AbstTask::~AbstTask() noexcept
    {
        force_quit(*this);
    }

    AbstTask::AbstTask(const AbstTask& _other) noexcept
        : interrupt_func{_other.interrupt_func}
    {
    }
    AbstTask& AbstTask::operator=(const AbstTask& _other) & noexcept
    {
        std::lock_guard<std::mutex> lock{m_machine_mutex};

        force_quit_task();
        interrupt_func = _other.interrupt_func;
        return *this;
    }
    AbstTask::AbstTask(AbstTask&& _other) noexcept
    {
        std::lock_guard<std::mutex> other_lock{_other.m_machine_mutex};

        _other.force_quit_task();
        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
    }
    AbstTask& AbstTask::operator=(AbstTask&& _other) & noexcept
    {
        std::lock_guard<std::mutex> lock{m_machine_mutex};
        force_quit_task();

        std::lock_guard<std::mutex> other_lock{_other.m_machine_mutex};
        _other.force_quit_task();

        interrupt_func = std::move(_other.interrupt_func);
        _other.interrupt_func = nullptr;
        return *this;
    }

    bool AbstTask::set_jump(int _priority, const std::shared_ptr<AbstTask>& _jump) noexcept
    {
        if (m_manager) {
            if (!m_manager->m_priority.has_value() || m_manager->m_priority.value() <= _priority) {
                m_manager->m_jump_task = _jump;
                m_manager->m_priority = _priority;

                return true;
            }
        }

        return false;
    }

    bool AbstTask::evaluate(AbstTask& _task)
    {
        return _task.evaluate_machine(m_manager);
    }

    bool AbstTask::evaluate_as_manager(AbstTask& _task)
    {
        {
            std::lock_guard<std::mutex> lock{m_machine_mutex};
            m_manager = {std::shared_ptr<AbstTask>{nullptr}, this};
        }
        return evaluate(_task);
    }

    bool AbstTask::evaluate_machine(std::shared_ptr<AbstTask>& _ptr)
    {
        std::lock_guard<std::mutex> lock{m_machine_mutex};

        // タスク評価前にマネージャー登録
        m_task_on_eval->m_manager = _ptr;
        auto result = m_task_on_eval->evaluate_task();
        m_task_on_eval->m_manager = nullptr;

        if (auto next = result.pointer()) {  // 次のタスクを指定
            m_task_on_eval = next;
            return false;

        } else if (!result) {  // 引き続き実行
            return false;

        } else {  // 終了
            m_task_on_eval = {std::shared_ptr<AbstTask>{nullptr}, this};
            return true;
        }
    }

    NextTask AbstTask::evaluate_task()
    {
        if (!m_me_on_eval) {
            init();
            m_me_on_eval = true;
        }

        if (auto result = eval()) {
            m_me_on_eval = false;
            quit();
            return result;
        }

        return false;
    }

    void AbstTask::force_quit(AbstTask& _task) noexcept
    {
        _task.force_quit_machine();
    }

    void AbstTask::force_quit_machine() noexcept
    {
        std::lock_guard<std::mutex> lock{m_machine_mutex};

        m_task_on_eval->force_quit_task();
        m_task_on_eval = {std::shared_ptr<AbstTask>{nullptr}, this};
    }

    // 各種コンストラクタで呼ばれる可能性が有るため、
    // 全タスククラスに影響が有ると考えると例外を出させたくない
    void AbstTask::force_quit_task() noexcept
    {
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
            auto finish = evaluate_as_manager(*m_machine_on_eval);

            if (m_jump_task) {
                force_quit(*m_machine_on_eval);
                m_machine_on_eval = std::move(m_jump_task);

            } else if (finish) {
                m_running = false;
                m_machine_on_eval = {std::shared_ptr<AbstTask>{nullptr}, this};
            }

            m_priority = std::nullopt;
            m_jump_task = nullptr;
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
        force_quit(*this);
    }

    bool AbstTask::running() noexcept
    {
        return m_running;
    }

}  // namespace Expr

}  // namespace TaskManager
