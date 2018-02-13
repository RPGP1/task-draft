#include "task_set.hpp"

namespace TaskManager
{

TaskSet::TaskSet() noexcept
    : m_task_list{},
      m_index{0},
      m_rector_list{}
{
}

void TaskSet::reconstruct()
{
    for (unsigned int i{0}; i < m_rector_list.size(); ++i) {
        m_task_list.at(i) = m_rector_list.at(i)(m_task_list.at(i));
    }
}

TaskSet::TaskSet(const TaskSet& _other)
    : AbstTask{_other},
      m_task_list{_other.m_task_list},
      m_index{0},
      m_rector_list{_other.m_rector_list}
{
    reconstruct();
}
TaskSet& TaskSet::operator=(const TaskSet& _other) &
{
    AbstTask::operator=(_other);
    m_task_list = _other.m_task_list;
    m_index = 0;
    m_rector_list = _other.m_rector_list;
    reconstruct();
    return *this;
}
TaskSet::TaskSet(TaskSet&& _other) noexcept
    : AbstTask{std::move(_other)},
      m_task_list{std::move(_other.m_task_list)},
      m_index{0},
      m_rector_list{std::move(_other.m_rector_list)}
{
}
TaskSet& TaskSet::operator=(TaskSet&& _other) & noexcept
{
    AbstTask::operator=(std::move(_other));
    m_task_list = std::move(_other.m_task_list);
    m_index = 0;
    m_rector_list = std::move(_other.m_rector_list);
    return *this;
}

void TaskSet::init() noexcept
{
    m_index = 0;
}
NextTask TaskSet::eval()
{
    // 全タスクの終了
    if (m_index >= m_task_list.size()) {
        return true;
    }

    // 管理しているタスクを実行
    //     コンストラクタを考えると、呼び出せないオブジェクトは存在しないはず
    // trueが返ってきたら次を実行
    if (evaluate(*m_task_list.at(m_index))) {
        ++m_index;
        return eval();
    }

    // falseが返ってきたら自分もfalseを返す
    // つまり次のサイクルで同じタスクを実行する
    return false;
}

void TaskSet::interrupt()
{
    force_quit(*m_task_list.at(m_index));
    quit();
}

}  // namespace TaskManager
