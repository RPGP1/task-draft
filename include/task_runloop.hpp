#pragma once

#include "./abst_task.hpp"

namespace TaskManager
{

template <typename LoopType>
class RunLoop : public Expr::AbstTask
{
private:
    std::shared_ptr<LoopType> m_loop;

public:
    template <typename... Args>
    explicit RunLoop(Args&&... _args) noexcept(std::is_nothrow_constructible<LoopType, Args...>::value);

    virtual ~RunLoop() noexcept {}

    RunLoop(const RunLoop&) noexcept;
    RunLoop& operator=(const RunLoop&) & noexcept;
    RunLoop(RunLoop&&) noexcept;
    RunLoop& operator=(RunLoop&&) & noexcept;

protected:
    void init() override;
    bool eval() override;

    void interrupt() override;
};

}  // namespace TaskManager

#include "task_runloop_source.hpp"
