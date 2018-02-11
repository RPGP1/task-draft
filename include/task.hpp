#pragma once

#include "./abst_task.hpp"

// TODO: 現状、nullptrを直接渡しても弾かれるが、受け入れるべきか

// TODO: explicit Task::operator bool()を実装するべきか

// TODO: 逆に、呼び出せない関数を渡されたら警告を発するべきか

namespace TaskManager
{

/*!
 * @brief 1つのstd::functionに対応するタスク
 * @detail std::function<void()>と、
 * std::function<bool()>からの明示的な変換を可能とする。
 * 変換元の関数オブジェクトをevalの中身とするが、
 * void()型を登録した際には常にtrueを返すとする。
 */
class Task : public Expr::AbstTask
{
private:
    std::function<bool()> m_function;

public:
    // 引数無しで呼び出せ、返り値がvoidの時のみ受け取る
    template <typename T, std::enable_if_t<std::is_same<void, decltype(std::declval<T>()())>::value, std::nullptr_t> = nullptr>
    Task(T&&);

    // 引数無しで呼び出せ、返り値がboolの時のみ受け取る
    template <typename T, std::enable_if_t<std::is_same<bool, decltype(std::declval<T>()())>::value, bool> = false>
    Task(T&&);

public:
    virtual ~Task() noexcept {}

    Task(const Task&) noexcept;
    Task& operator=(const Task&) & noexcept;
    Task(Task&&) noexcept;
    Task& operator=(Task&&) & noexcept;

    bool eval() override;
};


// 初期化lambdaキャプチャを使用
template <typename T, std::enable_if_t<std::is_same<void, decltype(std::declval<T>()())>::value, std::nullptr_t>>
Task::Task(T&& _func)
    : m_function{nullptr}  // いったんnullptr
{
    if (std::function<void()> tmp{std::forward<T>(_func)}) {
        m_function = [func = std::move(tmp)]() mutable  // _funcのoperator()がconstで無くても呼ぶ
        {
            func();
            return true;
        };
    }
}

template <typename T, std::enable_if_t<std::is_same<bool, decltype(std::declval<T>()())>::value, bool>>
Task::Task(T&& _func)
    : m_function{std::forward<T>(_func)}
{
}

}  // namespace TaskManager
