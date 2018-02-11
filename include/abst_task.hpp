/*!
 * @file    abst_task.hpp
 * @brief   タスクを扱う全クラスの親となる抽象クラスを定義する
 */

// TODO: abst_task.cpp 参照

#pragma once

#include <atomic>
#include <functional>
#include <mutex>

namespace TaskManager
{

namespace Expr
{

    /*!
     * @brief コピー用のクラス
     * @detail タスクの中にはコピーが出来ないものもある
     * その場合には、コピー用のクラスを特殊化を用いて登録する
     */
    template <typename T>
    struct for_copy {
        using type = T;
    };

    template <typename T>
    using for_copy_t = typename for_copy<T>::type;


    /*!
     * @brief タスクの抽象クラス
     */
    class AbstTask
    {
    private:
        bool m_on_eval;                //!< タスクが実行中かを示すフラグ
        std::atomic<bool> m_managing;  //!< タスク実行を管理中かを示すフラグ

        std::mutex m_eval_mutex;

    public:
        std::function<void()> interrupt_func;  //!< interruptの直後に呼ばれる

    public:
        AbstTask() noexcept;
        virtual ~AbstTask() noexcept {}

        // コピー・ムーブは可能だが、タスクのコピー・ムーブであり、実行情報は移動しない
        //     具体的には、m_on_evalなどは移動しない
        // これは、子孫クラスでも同様にするべきである
        // タスク実行中にコピーしたりムーブされたりするとinterruptとして扱われる
        AbstTask(const AbstTask&) noexcept;
        AbstTask& operator=(const AbstTask&) & noexcept;
        AbstTask(AbstTask&&) noexcept;
        AbstTask& operator=(AbstTask&&) & noexcept;

        /*!
         * @fn
         * @brief タスク管理側から呼ばれる関数は、実際にはこのメンバ関数である。
         * @detail initやquitの呼び出しなどを担当している。
         * ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもないが、
         * 他のタスクを実行する際にはこれを呼び出さなければならない。
         */
        bool evaluate();

        /*!
         * @fn
         * @brief タスク管理側からinterrupt時に呼ばれる関数は、実際にはこのメンバ関数である。
         * @detail interruptやinterrupt_funcの呼び出しなどを担当している。
         * ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもないが、
         * 他のタスクを強制的に停止する際にはこれを呼び出さなければならない。
         * なお、実行中で無ければ何もしない。
         */
        void force_quit() noexcept;


        void start() noexcept;
        void resume();
        void stop() noexcept;
        void reset() noexcept;

        bool running() noexcept;


    protected:
        // 以下、子クラスで(再)定義するメソッド

        /*!
         * @fn
         * @brief 自分の担当するタスクを実行する
         * @return 次の処理に移っていいか
         * @detail タスクを管理する側から呼ばれる主なメンバ関数。
         * trueを返すと、次のタスクに同サイクル内で移行する。
         * falseを返すと、同サイクルでのタスク処理を停止し、次のサイクルにもう一度自分が呼ばれる。
         */
        virtual bool eval() = 0;
        virtual void init() {}  //! @brief 自分の番が回ってきた時、evalの直前に呼ばれる。
        virtual void quit() {}  //! @brief evalがtrueを返した後1度だけ呼ばれる。

        /*!
         * @brief 実行中に終了させられた時に呼ばれる
         * Transitionで切り替わる時など
         */
        virtual void interrupt() { quit(); };
    };

}  // namespace Expr

}  // namespace TaskManager
