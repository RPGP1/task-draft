#include <map>

#include "./abst_task.hpp"
#include "./task_set.hpp"

namespace TaskManager
{

namespace Expr
{
    // ジャンプの起点となるタスク
    //     メインタスクを1連と、
    //     ジャンプ条件・ジャンプ先タスクのペアーを任意数組登録する
    // 登録されたメインタスクを実行しながら、
    // 毎サイクルの終わりにジャンプ条件を評価する。
    //     真を返した条件が有れば、対応するタスクへジャンプする
    class Jump : public AbstTask
    {
    private:
        class JumpManager
        {
            enum class JumpType : bool {
                OneWay = false,
                ReturnBack = true
            };

            using jump_cond_t = std::tuple<std::function<bool()>, JumpType, std::shared_ptr<TaskSet>>;
            using jump_cond_list_t = std::map<int, std::vector<jump_cond_t>>;

            std::shared_ptr<jump_cond_list_t> m_jump_list{std::make_shared<jump_cond_list_t>()};

        public:
            JumpManager() noexcept {}

            virtual ~JumpManager() noexcept {}

            JumpManager(const JumpManager&);
            JumpManager& operator=(const JumpManager&) &;
            JumpManager(JumpManager&&) noexcept = default;
            JumpManager& operator=(JumpManager&&) & noexcept = default;

        private:
            struct JumpTarget {
                int priority;
                JumpType type;
                std::shared_ptr<TaskSet> task_ptr;
            };

        public:
            std::optional<JumpTarget> check();

            class JumpManagerOperator
            {
                class JumpIfCondition
                {
                    std::shared_ptr<JumpManager> m_jump_manager;
                    int m_priority;
                    std::function<bool()> m_func;
                    std::shared_ptr<TaskSet> m_taskset;

                public:
                    JumpIfCondition(const std::shared_ptr<JumpManager>&, int _priority, const std::function<bool()>&, const std::shared_ptr<TaskSet>&) noexcept;
                    JumpIfCondition(const std::shared_ptr<JumpManager>&, int _priority, std::function<bool()>&&, const std::shared_ptr<TaskSet>&) noexcept;
                    JumpIfCondition(std::shared_ptr<JumpManager>&&, int _priority, const std::function<bool()>&, std::shared_ptr<TaskSet>&&) noexcept;
                    JumpIfCondition(std::shared_ptr<JumpManager>&&, int _priority, std::function<bool()>&&, std::shared_ptr<TaskSet>&&) noexcept;

                    virtual ~JumpIfCondition() noexcept {}

                    JumpIfCondition(const JumpIfCondition&) noexcept = default;
                    JumpIfCondition& operator=(const JumpIfCondition&) & noexcept = default;
                    JumpIfCondition(JumpIfCondition&&) noexcept = default;
                    JumpIfCondition& operator=(JumpIfCondition&&) & noexcept = default;

                    template <typename... TaskClasses>
                    auto operator()(TaskClasses&&...) const& -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>;
                    template <typename... TaskClasses>
                    auto operator()(TaskClasses&&...) && -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>;

                    Jump operator()(std::nullptr_t) const& noexcept;
                    Jump operator()(std::nullptr_t) && noexcept;
                };

                class JumpIfClass
                {
                    std::shared_ptr<JumpManager> m_jump_manager;
                    int m_priority = 0;
                    std::shared_ptr<TaskSet> m_taskset;

                public:
                    JumpIfClass(const std::shared_ptr<JumpManager>&, const std::shared_ptr<TaskSet>&) noexcept;

                    virtual ~JumpIfClass() noexcept {}

                    JumpIfClass(const JumpIfClass&) noexcept = default;
                    JumpIfClass& operator=(const JumpIfClass&) & noexcept = default;
                    JumpIfClass(JumpIfClass&&) noexcept = default;
                    JumpIfClass& operator=(JumpIfClass&&) & noexcept = default;

                    JumpIfClass operator[](int _priority) const& noexcept;
                    JumpIfClass operator[](int _priority) && noexcept;

                    JumpIfCondition operator[](const std::function<bool()>&) const& noexcept;
                    JumpIfCondition operator[](std::function<bool()>&&) const& noexcept;
                    JumpIfCondition operator()(const std::function<bool()>&) const& noexcept;
                    JumpIfCondition operator()(std::function<bool()>&&) const& noexcept;

                    JumpIfCondition operator[](const std::function<bool()>&) && noexcept;
                    JumpIfCondition operator[](std::function<bool()>&&) && noexcept;
                    JumpIfCondition operator()(const std::function<bool()>&) && noexcept;
                    JumpIfCondition operator()(std::function<bool()>&&) && noexcept;
                };

                class JumpBackIfCondition
                {
                    std::shared_ptr<JumpManager> m_jump_manager;
                    int m_priority;
                    std::function<bool()> m_func;
                    std::shared_ptr<TaskSet> m_taskset;

                public:
                    JumpBackIfCondition(const std::shared_ptr<JumpManager>&, int _priority, const std::function<bool()>&, const std::shared_ptr<TaskSet>&) noexcept;
                    JumpBackIfCondition(const std::shared_ptr<JumpManager>&, int _priority, std::function<bool()>&&, const std::shared_ptr<TaskSet>&) noexcept;
                    JumpBackIfCondition(std::shared_ptr<JumpManager>&&, int _priority, const std::function<bool()>&, std::shared_ptr<TaskSet>&&) noexcept;
                    JumpBackIfCondition(std::shared_ptr<JumpManager>&&, int _priority, std::function<bool()>&&, std::shared_ptr<TaskSet>&&) noexcept;

                    virtual ~JumpBackIfCondition() noexcept {}

                    JumpBackIfCondition(const JumpBackIfCondition&) noexcept = default;
                    JumpBackIfCondition& operator=(const JumpBackIfCondition&) & noexcept = default;
                    JumpBackIfCondition(JumpBackIfCondition&&) noexcept = default;
                    JumpBackIfCondition& operator=(JumpBackIfCondition&&) & noexcept = default;

                    template <typename... TaskClasses>
                    auto operator()(TaskClasses&&...) const& -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>;
                    template <typename... TaskClasses>
                    auto operator()(TaskClasses&&...) && -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>;

                    Jump operator()(std::nullptr_t) const& noexcept;
                    Jump operator()(std::nullptr_t) && noexcept;
                };

                class JumpBackIfClass
                {
                    std::shared_ptr<JumpManager> m_jump_manager;
                    int m_priority = 0;
                    std::shared_ptr<TaskSet> m_taskset;

                public:
                    JumpBackIfClass(const std::shared_ptr<JumpManager>& _jump_manager, const std::shared_ptr<TaskSet>& _taskset) noexcept;
                    JumpBackIfClass(std::shared_ptr<JumpManager>&& _jump_manager, std::shared_ptr<TaskSet>&& _taskset) noexcept;

                    virtual ~JumpBackIfClass() noexcept {}

                    JumpBackIfClass(const JumpBackIfClass&) noexcept = default;
                    JumpBackIfClass& operator=(const JumpBackIfClass&) & noexcept = default;
                    JumpBackIfClass(JumpBackIfClass&&) noexcept = default;
                    JumpBackIfClass& operator=(JumpBackIfClass&&) & noexcept = default;

                    JumpBackIfClass operator[](int _priority) const& noexcept;
                    JumpBackIfClass operator[](int _priority) && noexcept;

                    JumpBackIfCondition operator[](const std::function<bool()>&) const& noexcept;
                    JumpBackIfCondition operator[](std::function<bool()>&&) const& noexcept;
                    JumpBackIfCondition operator()(const std::function<bool()>&) const& noexcept;
                    JumpBackIfCondition operator()(std::function<bool()>&&) const& noexcept;

                    JumpBackIfCondition operator[](const std::function<bool()>&) && noexcept;
                    JumpBackIfCondition operator[](std::function<bool()>&&) && noexcept;
                    JumpBackIfCondition operator()(const std::function<bool()>&) && noexcept;
                    JumpBackIfCondition operator()(std::function<bool()>&&) && noexcept;
                };

            public:
                const JumpIfClass JumpIf;
                const JumpBackIfClass JumpBackIf;

                JumpManagerOperator(const std::shared_ptr<JumpManager>&, const std::shared_ptr<TaskSet>&) noexcept;
                JumpManagerOperator(std::shared_ptr<JumpManager>&&, std::shared_ptr<TaskSet>&&) noexcept;

                virtual ~JumpManagerOperator() noexcept {}

                JumpManagerOperator(const JumpManagerOperator&) noexcept = default;
                JumpManagerOperator& operator=(const JumpManagerOperator&) & noexcept = default;
                JumpManagerOperator(JumpManagerOperator&&) noexcept = default;
                JumpManagerOperator& operator=(JumpManagerOperator&&) & noexcept = default;
            };
        };

    public:
        class EmbeddedJump : public AbstTask
        {
            TaskSet m_taskset;
            std::shared_ptr<JumpManager> m_jump_manager;

        public:
            EmbeddedJump(const Jump&);
            EmbeddedJump(Jump&&) noexcept;

            virtual ~EmbeddedJump() noexcept {}

            EmbeddedJump(const EmbeddedJump&);
            EmbeddedJump& operator=(const EmbeddedJump&) &;
            EmbeddedJump(EmbeddedJump&&) noexcept;
            EmbeddedJump& operator=(EmbeddedJump&&) & noexcept;

        protected:
            NextTask eval() override;
            void interrupt() override;
        };

    private:
        std::shared_ptr<TaskSet> m_taskset;
        std::shared_ptr<JumpManager> m_jump_manager{std::make_shared<JumpManager>()};

    public:
        Jump(const TaskSet& _taskset) : m_taskset{std::make_shared<TaskSet>(_taskset)} {}
        Jump(TaskSet&& _taskset) noexcept : m_taskset{std::make_shared<TaskSet>(std::move(_taskset))} {}

    private:
        Jump(const std::shared_ptr<TaskSet>&, const std::shared_ptr<JumpManager>&) noexcept;
        Jump(std::shared_ptr<TaskSet>&&, std::shared_ptr<JumpManager>&&) noexcept;

    public:
        virtual ~Jump() noexcept {}

        Jump(const Jump&);
        Jump& operator=(const Jump&) &;
        Jump(Jump&&) noexcept;
        Jump& operator=(Jump&&) & noexcept;

        std::shared_ptr<JumpManager::JumpManagerOperator> operator->() const& noexcept;
        std::shared_ptr<JumpManager::JumpManagerOperator> operator->() && noexcept;

    protected:
        NextTask eval() override;
        void interrupt() override;
    };

    template <>
    struct set_for_copy<Jump> {
        using type = Jump::EmbeddedJump;
    };


    struct DuringOperator {
        template <typename... TaskClasses>
        Jump operator()(TaskClasses&&... _tasks) const
        {
            return Jump{TaskSet{std::forward<TaskClasses>(_tasks)...}};
        }
    };


    template <typename... TaskClasses>
    auto Jump::JumpManager::JumpManagerOperator::JumpIfCondition::operator()(TaskClasses&&... _tasks) const& -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(m_func, JumpType::OneWay, std::make_shared<TaskSet>(std::forward<TaskClasses>(_tasks)...));
            }
        }

        return {m_taskset, m_jump_manager};
    }
    template <typename... TaskClasses>
    auto Jump::JumpManager::JumpManagerOperator::JumpIfCondition::operator()(TaskClasses&&... _tasks) && -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(std::move(m_func), JumpType::OneWay, std::make_shared<TaskSet>(std::forward<TaskClasses>(_tasks)...));
            }
        }

        return {std::move(m_taskset), std::move(m_jump_manager)};
    }


    template <typename... TaskClasses>
    auto Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::operator()(TaskClasses&&... _tasks) const& -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(m_func, JumpType::ReturnBack, std::make_shared<TaskSet>(std::forward<TaskClasses>(_tasks)...));
            }
        }

        return {m_taskset, m_jump_manager};
    }
    template <typename... TaskClasses>
    auto Jump::JumpManager::JumpManagerOperator::JumpBackIfCondition::operator()(TaskClasses&&... _tasks) && -> std::enable_if_t<std::is_constructible<TaskSet, TaskClasses...>::value, Jump>
    {
        if (m_jump_manager) {
            if (auto& jump_list = m_jump_manager->m_jump_list) {
                (*jump_list)[m_priority].emplace_back(std::move(m_func), JumpType::ReturnBack, std::make_shared<TaskSet>(std::forward<TaskClasses>(_tasks)...));
            }
        }

        return {std::move(m_taskset), std::move(m_jump_manager)};
    }


}  // namespace Expr

constexpr Expr::DuringOperator During;

}  // namespace TaskManager
