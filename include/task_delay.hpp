#pragma once

#include "./abst_task.hpp"

namespace TaskManager
{

class Delay : public Expr::AbstTask
{
private:
    int m_delay;
    int m_count;

public:
    Delay(int _delay) noexcept;

    virtual ~Delay() noexcept {}

    Delay(const Delay&) noexcept;
    Delay& operator=(const Delay&) & noexcept;
    Delay(Delay&&) noexcept;
    Delay& operator=(Delay&&) & noexcept;

protected:
    void init() noexcept override;
    bool eval() noexcept override;
};

}  // namespace TaskManager
