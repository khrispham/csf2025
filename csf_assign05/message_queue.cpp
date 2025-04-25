#include "message_queue.h"
#include "guard.h"
#include <ctime>
#include <cerrno>
#include <iostream>

MessageQueue::MessageQueue() {
    pthread_mutex_init(&m_lock, nullptr);
    sem_init(&m_avail, 0, 0);
}

MessageQueue::~MessageQueue() {
    while (!m_messages.empty()) {
        delete m_messages.front();
        m_messages.pop_front();
    }
    pthread_mutex_destroy(&m_lock);
    sem_destroy(&m_avail);
}

void MessageQueue::enqueue(Message *msg) {
    Guard guard(m_lock);
    m_messages.push_back(msg);
    sem_post(&m_avail);
}

Message *MessageQueue::dequeue() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;

    if (sem_timedwait(&m_avail, &ts) == -1) {
        return nullptr;
    }

    Guard guard(m_lock);
    if (m_messages.empty()) {
        return nullptr;
    }

    Message *msg = m_messages.front();
    m_messages.pop_front();
    return msg;
}

int MessageQueue::get_sem_value(){
    int val;
    sem_getvalue(&m_avail, &val);
    return val;
}