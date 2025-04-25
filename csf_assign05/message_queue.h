#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <deque>
#include <pthread.h>
#include <semaphore.h>

struct Message;

class MessageQueue {
public:
    MessageQueue();
    ~MessageQueue();

    void enqueue(Message *msg);
    Message *dequeue();

    // Debugging function
    int get_sem_value();

private:
    MessageQueue(const MessageQueue &) = delete;
    MessageQueue &operator=(const MessageQueue &) = delete;

    pthread_mutex_t m_lock;
    sem_t m_avail;
    std::deque<Message *> m_messages;
};

#endif // MESSAGE_QUEUE_H