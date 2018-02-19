#pragma once

#include "./abst_task.hpp"
#include "./task_set.hpp"

namespace TaskManager
{

namespace Expr
{

    /*!
     * @brief Else節を持ったIf文
     * @detail 後述のIf文にElse節を付け加えたもの。
     * Else節を持ったIf文には、それ以上ElseIfやElseを付け加えられない。
     * その点で、IfはIfElseの拡張であると言え、
     * 実装でもIfをIfElseの子クラスとする。
     */
    class IfElse : public AbstTask
    {
    public:
        // clang-format off
        using condition_list_type = std::vector<
                                        std::pair<
                                            std::function<bool()>,
                                            TaskSet
                                        >
                                    >;  //!< (条件とTaskSetのペアー)のコンテナ
        // clang-format on
    protected:
        condition_list_type m_condition_list;               //!< 条件やタスクを格納する
        std::shared_ptr<TaskSet> m_selected_task{nullptr};  //!< 条件分岐の結果、実際に実行されるタスクを格納する。

    public:
        IfElse(const condition_list_type&);
        IfElse(condition_list_type&&) noexcept;

        virtual ~IfElse() noexcept {}

        IfElse(const IfElse&);
        IfElse& operator=(const IfElse&) &;
        IfElse(IfElse&&) noexcept;
        IfElse& operator=(IfElse&&) & noexcept;

    protected:
        /*!
         * @brief 条件分岐を行う
         * @detail 条件分岐の結果、実際に実行されることが決まったタスクはm_selected_taskに格納する
         */
        void init() override;
        /*!
         * @brief 然るべきタスクを実行する
         * @detail initの処理の結果に応じてm_selected_taskを実行する
         * @sa IfElse::init()
         */
        NextTask eval() override;

        void interrupt() override;
    };


    /*!
     * @brief Else節を持たないIf文
     */
    class If : public IfElse
    {
        class IfFunction
        {
            class ElseIfClass
            {
                class ElseIfCondition
                {
                private:
                    condition_list_type m_condition_list;
                    std::function<bool()> m_condition;

                public:
                    ElseIfCondition(const condition_list_type&, const std::function<bool()>&) noexcept;
                    ElseIfCondition(const condition_list_type&, std::function<bool()>&&) noexcept;

                    virtual ~ElseIfCondition() noexcept {}

                    ElseIfCondition(const ElseIfCondition&) = default;
                    ElseIfCondition& operator=(const ElseIfCondition&) & = default;
                    ElseIfCondition(ElseIfCondition&&) noexcept = default;
                    ElseIfCondition& operator=(ElseIfCondition&&) & noexcept = default;

                    template <typename... TaskClasses>
                    If operator()(TaskClasses&&... tasks) const&;
                    template <typename... TaskClasses>
                    If operator()(TaskClasses&&... tasks) &&;
                };

                std::shared_ptr<condition_list_type> m_cond_list_ptr;

            public:
                ElseIfClass(const std::shared_ptr<condition_list_type>& _ptr) noexcept : m_cond_list_ptr{_ptr} {}

                virtual ~ElseIfClass() noexcept {}

                ElseIfClass(const ElseIfClass&) noexcept = default;
                ElseIfClass& operator=(const ElseIfClass&) & noexcept = default;
                ElseIfClass(ElseIfClass&&) noexcept = default;
                ElseIfClass& operator=(ElseIfClass&&) & noexcept = default;

                ElseIfCondition operator[](const std::function<bool()>&) const noexcept;
                ElseIfCondition operator[](std::function<bool()>&&) const noexcept;

                ElseIfCondition operator()(const std::function<bool()>&) const noexcept;
                ElseIfCondition operator()(std::function<bool()>&&) const noexcept;
            };

        private:
            std::shared_ptr<condition_list_type> m_cond_list_ptr;

        public:
            const ElseIfClass ElseIf;

        public:
            IfFunction(const condition_list_type&);
            IfFunction(condition_list_type&&) noexcept;

            virtual ~IfFunction() noexcept {}

            IfFunction(const IfFunction&) noexcept = default;
            IfFunction& operator=(const IfFunction&) & noexcept = default;
            IfFunction(IfFunction&&) noexcept = default;
            IfFunction& operator=(IfFunction&&) & noexcept = default;

            template <typename... TaskClasses>
            IfElse Else(TaskClasses&&...);
        };

    public:
        If(const condition_list_type& _cond_list) : IfElse{_cond_list} {}
        If(condition_list_type&& _cond_list) noexcept : IfElse{std::move(_cond_list)} {}

        virtual ~If() noexcept {}

        If(const If& _other) : IfElse{_other} {}
        If& operator=(const If&) &;
        If(If&& _other) noexcept : IfElse{std::move(_other)} {}
        If& operator=(If&&) & noexcept;

        std::shared_ptr<IfFunction> operator->() const&;
        std::shared_ptr<IfFunction> operator->() && noexcept;
    };


    struct IfCondition {
    private:
        std::function<bool()> m_condition;

    public:
        IfCondition(const std::function<bool()>&) noexcept;
        IfCondition(std::function<bool()>&&) noexcept;

        virtual ~IfCondition() noexcept {}

        IfCondition(const IfCondition&) noexcept = default;
        IfCondition& operator=(const IfCondition&) & noexcept = default;
        IfCondition(IfCondition&&) noexcept = default;
        IfCondition& operator=(IfCondition&&) & noexcept = default;

        template <typename... TaskClasses>
        If operator()(TaskClasses&&...) const&;
        template <typename... TaskClasses>
        If operator()(TaskClasses&&...) &&;
    };


    struct IfOperator {
        IfCondition operator[](const std::function<bool()>&) const& noexcept;
        IfCondition operator[](std::function<bool()>&&) const& noexcept;
        IfCondition operator()(const std::function<bool()>&) const& noexcept;
        IfCondition operator()(std::function<bool()>&&) const& noexcept;
    };


    template <typename... TaskClasses>
    IfElse If::IfFunction::Else(TaskClasses&&... tasks)
    {
        condition_list_type tmp{*m_cond_list_ptr};
        tmp.emplace_back([] { return true; }, TaskSet{std::forward<TaskClasses>(tasks)...});
        return {tmp};
    }

    template <typename... TaskClasses>
    If If::IfFunction::ElseIfClass::ElseIfCondition::operator()(TaskClasses&&... tasks) const&
    {
        condition_list_type tmp{m_condition_list};
        if (m_condition) {
            tmp.emplace_back(m_condition, TaskSet{std::forward<TaskClasses>(tasks)...});
        }
        return {tmp};
    }
    template <typename... TaskClasses>
    If If::IfFunction::ElseIfClass::ElseIfCondition::operator()(TaskClasses&&... tasks) &&
    {
        if (m_condition) {
            m_condition_list.emplace_back(std::move(m_condition), TaskSet{std::forward<TaskClasses>(tasks)...});
        }
        m_condition = nullptr;
        return {std::move(m_condition_list)};
    }


    template <typename... TaskClasses>
    If IfCondition::operator()(TaskClasses&&... tasks) const&
    {
        if (m_condition) {
            return {{{m_condition, TaskSet{std::forward<TaskClasses>(tasks)...}}}};
        }
        return {{}};
    }
    template <typename... TaskClasses>
    If IfCondition::operator()(TaskClasses&&... tasks) &&
    {
        if (m_condition) {
            If tmp{{{std::move(m_condition), TaskSet{std::forward<TaskClasses>(tasks)...}}}};
            m_condition = nullptr;
            return tmp;
        }
        return {{}};
    }

}  // namespace Expr

constexpr Expr::IfOperator If;

}  // namespace TaskManager
