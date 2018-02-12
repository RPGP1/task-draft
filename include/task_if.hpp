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
        condition_list_type m_condition_list;  //!< 条件やタスクを格納する
        TaskSet* m_selected_task;              //!< 条件分岐の結果、実際に実行されるタスクを格納する。delete不要

    public:
        IfElse(const condition_list_type&);
        IfElse(condition_list_type&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);

        virtual ~IfElse() noexcept {}

        IfElse(const IfElse&);
        IfElse& operator=(const IfElse&) &;
        IfElse(IfElse&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);
        IfElse& operator=(IfElse&&) & noexcept(std::is_nothrow_move_constructible<TaskSet>::value);

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
    private:
        class ElseIfClass
        {
        private:
            class ElseIfCondition
            {
            private:
                std::shared_ptr<If> m_if_task;
                std::function<bool()> m_condition;

            public:
                ElseIfCondition(std::shared_ptr<If>&&, const std::function<bool()>&) noexcept;
                ElseIfCondition(std::shared_ptr<If>&&, std::function<bool()>&&) noexcept;

                virtual ~ElseIfCondition() noexcept {}

                ElseIfCondition(const ElseIfCondition&) noexcept;
                ElseIfCondition& operator=(const ElseIfCondition&) & noexcept;
                ElseIfCondition(ElseIfCondition&&) noexcept;
                ElseIfCondition& operator=(ElseIfCondition&&) & noexcept;

                template <typename... TaskClasses>
                std::shared_ptr<If> operator()(TaskClasses&&... tasks) const&;
                template <typename... TaskClasses>
                std::shared_ptr<If> operator()(TaskClasses&&... tasks) &&;
            };

            /*!
             * 親のIfクラスオブジェクトへのポインタ。
             * 循環参照を防ぐためweak_ptrを用いる。
             * なお、自分というオブジェクトが生きている限り、
             * コピー・ムーブが存在せず、コンストラクタが外部からはprivate contextなことから、
             * 親を指すweak_ptrの参照は有効なので、
             * 無効なweak_ptrを扱う危険性は考えなくてよい。
             */
            std::weak_ptr<If> m_parent_ptr;

        public:
            ElseIfClass() noexcept;

            void set_parent(const std::shared_ptr<If>&) noexcept;

            virtual ~ElseIfClass() noexcept {}

            ElseIfClass(const ElseIfClass&) = delete;
            ElseIfClass& operator=(const ElseIfClass&) = delete;
            ElseIfClass(ElseIfClass&&) = delete;
            ElseIfClass& operator=(ElseIfClass&&) = delete;

            ElseIfCondition operator[](const std::function<bool()>&) const noexcept;
            ElseIfCondition operator[](std::function<bool()>&&) const noexcept;

            ElseIfCondition operator()(const std::function<bool()>&) const noexcept;
            ElseIfCondition operator()(std::function<bool()>&&) const noexcept;
        };

    public:
        ElseIfClass ElseIf;

    private:
        /*!
         * 自分自身へのポインタ。
         * 循環参照を防ぐためweak_ptrを用いる。
         * なお、自分というオブジェクトが生きている限りweak_ptrの参照は有効なので、
         * 無効なweak_ptrを扱う危険性は考えなくてよい。
         */
        std::weak_ptr<If> m_this_ptr;

    public:
        static std::shared_ptr<If> create(const condition_list_type&);
        static std::shared_ptr<If> create(condition_list_type&&) noexcept(std::is_nothrow_constructible<If, condition_list_type&&>::value);

    private:
        If(const condition_list_type&);
        If(condition_list_type&&) noexcept(std::is_nothrow_constructible<IfElse, condition_list_type&&>::value);

        void initialize_ptr(const std::shared_ptr<If>&) noexcept;

    public:
        virtual ~If() noexcept {}

        If(const If&) = delete;
        If& operator=(const If&) &;
        If(If&&) = delete;
        If& operator=(If&&) & noexcept(std::is_nothrow_move_assignable<TaskSet>::value);

        std::shared_ptr<If> copy();

        template <typename... TaskClasses>
        std::shared_ptr<IfElse> Else(TaskClasses&&... tasks);

    private:
        void add_condition(const condition_list_type::value_type&);
        void add_condition(condition_list_type::value_type&&) noexcept(std::is_nothrow_move_constructible<TaskSet>::value);
    };


    // Ifはコピーできないので、
    // TaskSetに入れるなどのコピーする時にはIfElseにコピーする
    template <>
    struct for_copy<If> {
        using type = IfElse;
    };


    struct IfCondition {
    private:
        std::function<bool()> m_condition;

    public:
        IfCondition(const std::function<bool()>&) noexcept;
        IfCondition(std::function<bool()>&&) noexcept;

        virtual ~IfCondition() noexcept {}

        IfCondition(const IfCondition&) noexcept;
        IfCondition& operator=(const IfCondition&) & noexcept;
        IfCondition(IfCondition&&) noexcept;
        IfCondition& operator=(IfCondition&&) & noexcept;

        template <typename... TaskClasses>
        std::shared_ptr<If> operator()(TaskClasses&&...) const&;
        template <typename... TaskClasses>
        std::shared_ptr<If> operator()(TaskClasses&&...) &&;
    };


    struct IfOperator {
        IfCondition operator[](const std::function<bool()>&) const& noexcept;
        IfCondition operator[](std::function<bool()>&&) const& noexcept;
        IfCondition operator()(const std::function<bool()>&) const& noexcept;
        IfCondition operator()(std::function<bool()>&&) const& noexcept;
    };


    template <typename... TaskClasses>
    std::shared_ptr<IfElse> If::Else(TaskClasses&&... tasks)
    {
        add_condition({[] { return true; }, TaskSet{std::forward<TaskClasses>(tasks)...}});
        return m_this_ptr.lock();
    }

    template <typename... TaskClasses>
    std::shared_ptr<If> If::ElseIfClass::ElseIfCondition::operator()(TaskClasses&&... tasks) const&
    {
        std::shared_ptr<If> tmp{m_if_task};
        if (m_condition && sizeof...(TaskClasses) > 0) {
            tmp->add_condition({m_condition, TaskSet{std::forward<TaskClasses>(tasks)...}});
        }
        return tmp;
    }
    template <typename... TaskClasses>
    std::shared_ptr<If> If::ElseIfClass::ElseIfCondition::operator()(TaskClasses&&... tasks) &&
    {
        std::shared_ptr<If> tmp{std::move(m_if_task)};
        m_if_task = nullptr;
        if (m_condition && sizeof...(TaskClasses) > 0) {
            tmp->add_condition({std::move(m_condition), TaskSet{std::forward<TaskClasses>(tasks)...}});
            m_condition = nullptr;
        }
        return tmp;
    }


    template <typename... TaskClasses>
    std::shared_ptr<If> IfCondition::operator()(TaskClasses&&... tasks) const&
    {
        if (m_condition && sizeof...(TaskClasses) > 0) {
            return If::create({{m_condition, TaskSet{std::forward<TaskClasses>(tasks)...}}});
        }
        return If::create({});
    }
    template <typename... TaskClasses>
    std::shared_ptr<If> IfCondition::operator()(TaskClasses&&... tasks) &&
    {
        if (m_condition && sizeof...(TaskClasses) > 0) {
            auto&& tmp = If::create({{std::move(m_condition), TaskSet{std::forward<TaskClasses>(tasks)...}}});
            m_condition = nullptr;
            return tmp;
        }
        return If::create({});
    }

}  // namespace Expr

constexpr Expr::IfOperator If;

}  // namespace TaskManager
