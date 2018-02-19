/*!
 * @file    abst_task.hpp
 * @brief   タスクを扱う全クラスの親となる抽象クラスを定義する
 */

// TODO: abst_task.cpp 参照

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>

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
    struct set_for_copy {
        using type = T;
    };

    namespace
    {
        template <typename T, bool = std::is_same<T, typename set_for_copy<T>::type>::value>
        struct for_copy_recursive;

        template <typename T>
        struct for_copy_recursive<T, true> {
            using type = T;
        };

        template <typename T>
        struct for_copy_recursive<T, false> {
            using type = typename for_copy_recursive<typename set_for_copy<T>::type>::type;
        };
    }

    template <typename T>
    using for_copy_t = typename for_copy_recursive<T>::type;


    class AbstTask;
    /*!
     * AbstTask::eval()、AbstTask::evaluate_task()の返り値の型
     * 引き続き自タスクの処理を続けるならfalse、
     * 何か別のタスクを指定して、そちらに移るならタスクのshared_ptr、
     * そうではなく終了するならnullptrかtrueでコンストラクト
     */
    struct NextTask {
    private:
        bool m_finish = true;
        std::shared_ptr<AbstTask> m_next = nullptr;

    public:
        template <typename T, std::enable_if_t<std::is_same<T, bool>::value, std::nullptr_t> = nullptr>
        NextTask(T _finish) noexcept : m_finish{_finish}
        {
        }
        NextTask(const std::shared_ptr<AbstTask>& _next) noexcept : m_next{_next} {}
        NextTask(std::shared_ptr<AbstTask>&& _next) noexcept : m_next{std::move(_next)} {}

        NextTask(const NextTask&) = delete;
        NextTask& operator=(const NextTask&) = delete;
        NextTask(NextTask&&) noexcept;
        NextTask& operator=(NextTask&&) & noexcept;

        // 終了するか
        explicit operator bool() const noexcept { return m_next || m_finish; }
        std::shared_ptr<AbstTask> pointer() const noexcept { return m_next; }
    };


    /*!
     * @brief タスクの抽象クラス
     * @detail 各タスクを定義する為のインターフェースを提供するとともに、
     * 実行処理に関係するメソッドを定義する。
     * 
     * AbstTaskはシーケンス制御の中の1タスクであると同時に、
     * そのシーケンス処理の命令塔にもなり、
     * シーケンス制御中にステートマシン処理を組み込んだ際には、それの命令塔にもなる。
     * 
     * 以下のドキュメントでは、
     * 「シーケンス制御の中の1タスク」としての自身を「(この)タスク」と、
     * 「シーケンス処理の命令塔」としての自身を「(この)マネージャー」と、
     * 「ステートマシン処理の命令塔」としての自身を「(この)マシン」と表記する。
     * 
     * タスクを定義する時にはマネージャーとマシンの違いを意識せずに済み、
     * タスクを呼び出す時にはマシンとタスクの違いを意識せずに済むようにすることが目的。
     */
    class AbstTask
    {
    private:
        bool m_me_on_eval{false};                                                               //!< このタスクが実行中かを示すフラグ
        std::shared_ptr<AbstTask> m_manager{nullptr};                                           //!< このタスクを管理しているマネージャーのポインタ
        std::shared_ptr<AbstTask> m_task_on_eval{std::shared_ptr<AbstTask>{nullptr}, this};     //!< このマシンが実行しているタスクのポインタ
        std::atomic<bool> m_running{false};                                                     //!< このマネージャーがタスク処理を呼び出す状態かを示すフラグ
        std::shared_ptr<AbstTask> m_machine_on_eval{std::shared_ptr<AbstTask>{nullptr}, this};  //!< このマネージャーが処理を呼び出すマシンのポインタ
        std::optional<int> m_priority{std::nullopt};                                            //!< ジャンプ判定で用いる優先度
        std::shared_ptr<AbstTask> m_jump_task{nullptr};                                         //!< ジャンプ判定が真になったタスク

        std::mutex m_machine_mutex;  //!< タスクの実行と中断処理が同時に行われないようにするmutex

    public:
        std::function<void()> interrupt_func{nullptr};  //!< このタスクのinterruptの直後に呼ばれる

    public:
        AbstTask() noexcept {}
        virtual ~AbstTask() noexcept;

        // コピー・ムーブは可能だが、タスクの処理内容のコピー・ムーブであり、実行情報は移動しない
        //     具体的には、interrupt_funcしか移動しない
        // これは、子孫クラスでも同様にするべきである
        // 万が一、タスク実行中にコピーしたりムーブされたりするとinterruptとして扱われる
        //     evaluateやforce_quitを実行中のスレッド内でinterruptになると身動きできず死ぬ！！！
        //         「*this = ...」なんて実行する奴が悪い
        AbstTask(const AbstTask&) noexcept;
        AbstTask& operator=(const AbstTask&) & noexcept;
        AbstTask(AbstTask&&) noexcept;
        AbstTask& operator=(AbstTask&&) & noexcept;

    protected:
        bool set_jump(int _priority, const std::shared_ptr<AbstTask>&) noexcept;

        /*!
         * @fn
         * @brief マネージャーやタスク管理側からこのマシン・タスクの内容として呼ばれる関数は、実際にはこのメンバ関数である。
         * @detail マネージャー情報を伝達し、タスクの処理を呼ぶなどしている。
         * ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもないが、
         * 他のタスクを実行する際にはこれを呼び出さなければならない。
         * @return 実行したタスクが終了したか。trueなら終了の意。
         */
        bool evaluate(AbstTask&);

    private:
        /*!
         * @fn
         * @brief マネージャーとしてタスクを呼び出す
         * @detail m_managerを設定してからevaluate(AbstTask&)を呼び出す。
         * @return evaluate(AbstTask&)と同様。
         * @sa evaluate(AbstTask&);
         */
        bool evaluate_as_manager(AbstTask&);
        /*!
         * @fn
         * @brief マシンに登録されたタスクへ評価のバトンを渡す関数。
         * @detail マシンとしての処理を行い、タスクの処理を呼ぶなどしている。
         * ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもない。
         * evaluate(AbstTask&)から呼び出される。
         */
        bool evaluate_machine(std::shared_ptr<AbstTask>&);
        /*!
         * @fn
         * @brief このタスクに定義されたeval()やinit()、quit()を状況に応じて呼び出す関数
         * @detail ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもない。
         * evaluate_machine(std::shared_ptr<AbstTask>&)から呼び出される。
         */
        NextTask evaluate_task();

    protected:
        /*!
         * @fn
         * @brief タスク管理側からこのマシン・タスクのinterrupt時に呼ばれる関数は、実際にはこのメンバ関数である。
         * @detail マシンの中断処理を呼ぶ。
         * ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもないが、
         * 他のタスクを強制的に停止する際にはこれを呼び出さなければならない。
         * なお、実行中で無ければ何もしない。
         */
        void force_quit(AbstTask&) noexcept;

    private:
        /*!
         * @fn
         * @brief タスク管理側からこのマシン・タスクのinterrupt時に呼ばれる関数は、実際にはこのメンバ関数である。
         * @detail マシンとしての処理を行い、タスクの中断処理を呼ぶ。
         * @detail ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもない。
         * force_quit(AbstTask&)から呼び出される。
         */
        void force_quit_machine() noexcept;
        /*!
         * @fn
         * @brief このタスクに定義された中断処理を状況に応じて呼び出す関数
         * @detail ユーザは気にしなくてよいし、virtualでないことから分かるように編集するべきものでもない。
         * force_quit_machine()から呼び出される。
         */
        void force_quit_task() noexcept;


    public:
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
         * @detail タスクを管理する側から呼ばれる主なメンバ関数。
         * @return 返り値の意味はNextTaskクラス参照
         * @sa NextTask
         */
        virtual NextTask eval() = 0;
        virtual void init() {}  //! @brief 自分の番が回ってきた時、evalの直前に呼ばれる。
        virtual void quit() {}  //! @brief evalが終了を示した後1度だけ呼ばれる。

        /*!
         * @brief 実行中に終了させられた時に呼ばれる
         * Transitionで切り替わる時など
         */
        virtual void interrupt() { quit(); };
    };

}  // namespace Expr

// タスクはシーン(ステート)の機能も持つ。
// ならばAbstTaskはAbstSceneでもある。
using AbstScene = Expr::AbstTask;

using NextTask = Expr::NextTask;

}  // namespace TaskManager
