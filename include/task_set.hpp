#pragma once

#include <memory>
#include <vector>

#include "./abst_task.hpp"
#include "./task.hpp"

namespace TaskManager
{

/*!
 * @brief 複数のタスクをまとめるクラス
 * @detail AbstTaskの子孫クラスのオブジェクトと、
 * それらのポインタ型のオブジェクトと、
 * Taskに変換できる関数オブジェクトをまとめてコンテナに入れる。
 * その際、各オブジェクトはコピーされる。
 */
class TaskSet : public Expr::AbstTask
{
private:
    std::vector<std::shared_ptr<AbstTask>> m_task_list;  //! タスクを管理するリスト
    decltype(m_task_list)::size_type m_index;            //! 実行中のタスクのインデックス

    // AbstTaskのpublic子孫の実体型に対してのみ使用せよ
    template <typename T>
    struct reconstructor {
        std::shared_ptr<AbstTask> operator()(const std::shared_ptr<AbstTask>& _ptr) noexcept(std::is_nothrow_copy_constructible<T>::value)
        {
            return std::shared_ptr<AbstTask>{new T{*dynamic_cast<const T*>(_ptr.get())}};
        }
    };

    using task_reconstructor_t = std::function<std::shared_ptr<AbstTask>(const std::shared_ptr<AbstTask>&)>;

    std::vector<task_reconstructor_t> m_rector_list;

    template <typename T, typename...>
    struct first_parameter {
        using type = T;
    };

public:
    template <typename... Ts>
    using first_parameter_t = typename first_parameter<Ts...>::type;

    // 下のメタ関数により、引数が1つもないとErrorになるので、回避する
    TaskSet() noexcept;

    // コピーやムーブを除外する
    //     引数1つで、const TaskSet&やTaskSet&&と見なせるときは除外
    // clang-format off
    template <typename... TaskClasses,
        std::enable_if_t<
            !(sizeof...(TaskClasses) == 1
            && (
                std::is_convertible<
                    first_parameter_t<TaskClasses...>,
                    const TaskSet&
                >::value
                || std::is_convertible<
                    first_parameter_t<TaskClasses...>,
                    TaskSet&&
                >::value
            )),
            std::nullptr_t
    > = nullptr>
    // clang-format on
    explicit TaskSet(TaskClasses&&...);

private:
    void construct() const noexcept {}

    /*!
     * T型のオブジェクトが引数無しでoperator()を呼べる関数オブジェクトであり、
     * std::functionに変換してTaskのコンストラクタに渡せる時のみ定義される
     * 
     * 例: []{}, std::function<bool()>
     */
    // clang-format off
    template <typename T, std::enable_if_t<
        std::is_constructible<
            Task,
            decltype(
                std::function<
                    decltype(std::declval<T>()())
                    ()
                >{
                    std::declval<T>()
                }
            )
        >::value,
        std::nullptr_t
    > = nullptr>
    // clang-format on
    // = template<typename T, std::nullptr_t>
    void construct_one(T&& _func);

    /*!
     * T型のオブジェクトxが前置operator*を呼べ、
     * その返り値のcvr修飾をとった型をU、
     * using A = for_copy_t<U>とすると、
     * コピーコンストラクトA(*x)が可能であり、
     * A*がAbstTask*に暗黙的に変換できるときのみ定義される
     * 
     * 例: std::shared_ptr<Task>, const Task*
     */
    // clang-format off
    template <typename T,
        typename element_type = std::enable_if_t<
            std::is_convertible<
                decltype(
                    new Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<
                        decltype(*std::declval<T>())
                    >>>{
                        *std::declval<T>()
                    }
                ),
                AbstTask*
            >::value,
            Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<
                decltype(*std::declval<T>())
            >>>
        >
    >
    // clang-format on
    // = template<typename T, typename element_type>
    void construct_one(T&& _ptr);

    /*!
     * T型のオブジェクトxが、
     * そのcvr修飾をとった型をU、
     * using A = for_copy_t<U>とすると、
     * コピーコンストラクトA(x)が可能であり、
     * A*がAbstTask*に暗黙的に変換できるときのみ定義される
     * 
     * 例: const Task&, Task&&
     */
    // clang-format off
    template <typename T,
        typename task_type = Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<T>>>,
        std::enable_if_t<
            std::is_convertible<
                decltype(
                    new task_type{std::declval<T>()}
                ),
                AbstTask*
            >::value,
            std::nullptr_t
        > = nullptr
    >
    // clang-format on
    // = template<typename T, typename task_type, std::nullptr_t>
    void construct_one(T&& _task);

    template <typename T, typename... TaskClasses>
    void construct(T&&, TaskClasses&&...);

    void reconstruct();

public:
    virtual ~TaskSet() noexcept {}

    TaskSet(const TaskSet&);
    TaskSet& operator=(const TaskSet&) &;
    TaskSet(TaskSet&&) noexcept;
    TaskSet& operator=(TaskSet&&) & noexcept;

protected:
    void init() noexcept override;
    NextTask eval() override;

    void interrupt() override;
};

}  // namespace TaskManager

#include "task_set_source.hpp"
