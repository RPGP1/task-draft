#pragma once

#include "./task_set.hpp"

namespace TaskManager
{

template <typename... TaskClasses, std::enable_if_t<!(sizeof...(TaskClasses) == 1 && (std::is_convertible<TaskSet::first_parameter_t<TaskClasses...>, const TaskSet&>::value || std::is_convertible<TaskSet::first_parameter_t<TaskClasses...>, TaskSet&&>::value)), std::nullptr_t>>
TaskSet::TaskSet(TaskClasses&&... tasks)
    : m_task_list{},
      m_index{0},
      m_rector_list{}
{
    m_task_list.reserve(sizeof...(TaskClasses));
    m_rector_list.reserve(sizeof...(TaskClasses));
    construct(std::forward<TaskClasses>(tasks)...);
}

template <typename T, std::enable_if_t<std::is_constructible<Task, decltype(std::function<decltype(std::declval<T>()())()>{std::declval<T>()})>::value, std::nullptr_t>>
void TaskSet::construct_one(T&& _func)
{
    // std::functionに入る型なら、呼び出せないオブジェクトでも格納できる
    std::function<decltype(std::declval<T>()())()> tmp_func{std::forward<T>(_func)};

    if (tmp_func) {  // 呼び出せないものを登録しない
        m_task_list.push_back(std::make_shared<Task>(std::move(tmp_func)));
        m_rector_list.push_back(reconstructor<Task>{});
    }
}

template <typename T, typename element_type = std::enable_if_t<std::is_convertible<decltype(new Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<T>())>>>{*std::declval<T>()}), Expr::AbstTask*>::value, Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<T>())>>>>>
void TaskSet::construct_one(T&& _ptr)
{
    // nullptrを除外
    // operator bool()の無い自作ポインタだとコンパイルエラー
    if (_ptr) {
        m_task_list.push_back(std::make_shared<element_type>(*_ptr));
        m_rector_list.push_back(reconstructor<element_type>{});
    }
}

template <typename T, typename task_type = Expr::for_copy_t<std::remove_cv_t<std::remove_reference_t<T>>>, std::enable_if_t<std::is_convertible<decltype(new task_type{std::declval<T>()}), Expr::AbstTask*>::value, std::nullptr_t>>
void TaskSet::construct_one(T&& _task)
{
    m_task_list.push_back(std::make_shared<task_type>(std::forward<T>(_task)));
    m_rector_list.push_back(reconstructor<task_type>{});
}

template <typename T, typename... TaskClasses>
void TaskSet::construct(T&& _task, TaskClasses&&... tasks)
{
    construct_one(std::forward<T>(_task));
    construct(std::forward<TaskClasses>(tasks)...);
}

}  // namespace TaskManager
