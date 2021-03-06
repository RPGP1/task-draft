#pragma once

#include "./task_while.hpp"

namespace TaskManager
{

namespace Expr
{

    class DoWhile : public While
    {
    public:
        DoWhile(const TaskSet&, const std::function<bool()>&);
        DoWhile(const TaskSet&, std::function<bool()>&&);
        DoWhile(TaskSet&&, const std::function<bool()>&) noexcept(std::is_nothrow_constructible<While, const std::function<bool()>&, TaskSet&&>::value);
        DoWhile(TaskSet&&, std::function<bool()>&&) noexcept(std::is_nothrow_constructible<While, std::function<bool()>&&, TaskSet&&>::value);

        virtual ~DoWhile() noexcept {}

        DoWhile(const DoWhile&);
        DoWhile& operator=(const DoWhile&) &;
        DoWhile(DoWhile&&) noexcept(std::is_nothrow_move_constructible<While>::value);
        DoWhile& operator=(DoWhile&&) & noexcept(std::is_nothrow_move_assignable<While>::value);

    protected:
        void init() noexcept override {}
        NextTask eval() override;
    };


    class DoTaskSet
    {
    private:
        class WhileClass
        {
        private:
            std::shared_ptr<TaskSet> m_taskset;

        public:
            WhileClass(const std::shared_ptr<TaskSet>&) noexcept;
            WhileClass(std::shared_ptr<TaskSet>&&) noexcept;

            virtual ~WhileClass() noexcept {}

            WhileClass(const WhileClass&) noexcept = default;
            WhileClass& operator=(const WhileClass&) & noexcept = default;
            WhileClass(WhileClass&&) noexcept = default;
            WhileClass& operator=(WhileClass&&) & noexcept = default;

            DoWhile operator[](const std::function<bool()>&) const;
            DoWhile operator[](std::function<bool()>&&) const;
            DoWhile operator()(const std::function<bool()>&) const;
            DoWhile operator()(std::function<bool()>&&) const;
        };

        class UntilClass : public WhileClass
        {
        public:
            UntilClass(const std::shared_ptr<TaskSet>&) noexcept;
            UntilClass(std::shared_ptr<TaskSet>&&) noexcept;

            virtual ~UntilClass() noexcept {}

            UntilClass(const UntilClass&) noexcept;
            UntilClass& operator=(const UntilClass&) & noexcept;
            UntilClass(UntilClass&&) noexcept;
            UntilClass& operator=(UntilClass&&) & noexcept;

            DoWhile operator[](const std::function<bool()>&) const;
            DoWhile operator[](std::function<bool()>&&) const;
            DoWhile operator()(const std::function<bool()>&) const;
            DoWhile operator()(std::function<bool()>&&) const;
        };

    public:
        const WhileClass While;
        const UntilClass Until;

    public:
        DoTaskSet(const std::shared_ptr<TaskSet>&) noexcept;
        DoTaskSet(std::shared_ptr<TaskSet>&&) noexcept;

        virtual ~DoTaskSet() noexcept {}

        DoTaskSet(const DoTaskSet&) noexcept = default;
        DoTaskSet& operator=(const DoTaskSet&) & noexcept = default;
        DoTaskSet(DoTaskSet&&) noexcept = default;
        DoTaskSet& operator=(DoTaskSet&&) & noexcept = default;
    };


    struct DoOperator {
        template <typename... TaskClasses>
        std::shared_ptr<DoTaskSet> operator()(TaskClasses&&... tasks) const
        {
            return std::make_shared<DoTaskSet>(std::make_shared<TaskSet>(std::forward<TaskClasses>(tasks)...));
        }
    };

}  // namespace Expr

constexpr Expr::DoOperator Do{};

}  // namespace TaskManager
