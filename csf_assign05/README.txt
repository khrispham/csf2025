Sample README.txt

Eventually your report about how you implemented thread synchronization
in the server should go here

Member contribution:
John implemented worker (in server) and room functionality
Khris implemented chat_with_receiver/sender and message_queue functionality

Implementation report:
Each Room and the Server class contains their own pthread_mutex_t, with Guard
prefacing any functions that involves room manipulation, such as finding/creating
rooms and adding/removing members.

MessageQueue implements a sender-receiver pattern using a mutex and semaphore
pair. The mutex safeguards the underlying message dequeue during enqueue operations,
while the semaphore tracks available messages for receivers. During broadcasts,
senders first acquire their Room’s mutex to ensure stable membership lists, then
briefly lock each receiver’s MessageQueue to insert messages. The Room lock is
released after these operations.

There is a 1 second time-out on dequeues to prevent indefinite blocking if no messages
arrive. There is a consistent lock acquisition order (Room->User->MessageQueue) to
avoid deadlocks.