Synchronization report:

Dayana: Message_queue.cpp, room.cpp, communicate_with_sender helper function in server.cpp
Tobi: All of Server.cpp, helped with communicate_with_sender helper function in server.cpp. Wrote report on thread synchronization as well. 


The main focus was making sure that the queues were thread safe and there are multiple
files where we made to put guardrails in place. The files that worked to ensure thread syncrhonization and
thread safe queues were in message_queue.cpp, room.cpp, and server.cpp. 

For message_queue.cpp we used pthread_mutex_t and sem_t (the semephore) to make sure that in the enqueue 
process, the mutex is locked to prevent the message list from being corrupted (maybe with duplicate accesses or deletions);
the sem_post is called to signal availability after unlocking. With the dequeueing process sem_timedwait is used to wait for a message
and once it's signaled, the mutex is acuired to remove the message from the list of messages. Rather than blocking the thread forever
sem_timedwait helps the thread wake up periodically to check for connection issues.

In Room.cpp pthread_mutex_t managed by the Guard object helps make sure that in adding and removing a member from a room
the mutex is locked at the appropriate time. Additionally when the message is being broadcasted to all the members in a room
the mutex is held while iterating through the member set. The mutex is held in the case of a user trying to leave the room
while anothe rthread was iterating thruogh the member set to broadcast a message; in that case the iterator would fail and the 
server would crash so holding the mutex during the broadcast helps prevents removals during broadcasting. Something else to note is that
messages passed thread to thread through the messagequeue are allocated on the heap rather than the stack to make sure the
data is still valid even after the creating thread moves on to other tasks. 

In server.cpp, specifically in the find_or_create_room function guard locks the mutex when the function is called
in the case that multiple clients tries to join (or create) the same room simultaneously the mutex locks to prevent duplicate memory allocations or
<<<<<<< HEAD
<<<<<<< HEAD
m_rooms map corruption.
=======
m_rooms map corruption.
>>>>>>> 1c4498d (sync report)
=======
m_rooms map corruption.
>>>>>>> 1c4498d (sync report)
