#include <inc/time_seq.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

/**
 * For each tail queue queue_ptr in time_seq,
 * apply func(queue_ptr)
 */
static void time_seq_map(struct time_seq_t *time_seq, void (* func) (struct event_queue_head_t *))
{
	int i;
	for (i = 0; i < time_seq->length; i++)
		func(&(time_seq->queue_array_ptr[i]));
}
/**
 * destory a TAILQ
 * XXX: I do not remove any queue member (if any exists)
 * 		This should be done at time_seq_destory_at()
 */

void time_seq_destory(struct time_seq_t *time_seq)
{
	free(time_seq -> queue_array_ptr);
}

/**
 * initialize a TAILQ
 * will be called by time_seq_map()
 */

static void time_seq_init_mapper(struct event_queue_head_t *queue)
{
	//*queue=TAILQ_HEAD_INITIALIZER(*queue);
	/**
	 * XXX: the following code are from TAILQ_HEAD_INITIALIZER
	 */
	queue->tqh_first = NULL;
	queue->tqh_last = &(queue->tqh_first);
	TAILQ_INIT(queue);
}

struct time_seq_t* time_seq_init(size_t total_millisecond)
{
	struct event_queue_head_t *ptr;
	struct time_seq_t *rtn;
	rtn = (struct time_seq_t *)malloc(sizeof(struct time_seq_t));
	ptr = (struct event_queue_head_t *)malloc(sizeof(struct event_queue_head_t) * total_millisecond);
	rtn -> queue_array_ptr = ptr;
	rtn -> length = total_millisecond;
	time_seq_map(rtn, time_seq_init_mapper);
	return rtn;
}

/**
 * insert an event at the queue head, at time
 */
inline void time_seq_insert_head(struct time_seq_t *time_seq, size_t time, struct event_t *event)
{
	assert(time < time_seq -> length);
	struct event_queue_head_t *queue = time_seq -> queue_array_ptr + time;

	TAILQ_INSERT_HEAD(queue, event, queue_entry);
}

/**
 * insert an event at the queue tail, at time
 */
inline void time_seq_insert_tail(struct time_seq_t *time_seq, size_t time, struct event_t *event)
{
	assert(time < time_seq -> length);
	TAILQ_INSERT_TAIL(time_seq->queue_array_ptr + time, event, queue_entry);
}

/**
 * trigger the events in order, at time
 */

void time_seq_trigger_at(struct time_seq_t *time_seq, size_t time)
{
	struct event_t *event;
	struct event_queue_head_t *queue_head = time_seq -> queue_array_ptr + time;
	static int no_more_trigger = 0;
	if(time >= time_seq -> length){
		if (!no_more_trigger)
			printf("time_seq: No more triggers!\n");
		no_more_trigger = 1;
		return;
	}

	TAILQ_FOREACH(event, queue_head, queue_entry)
			event->trigger(event, time_seq, time);
}

/**
 * remove and free all events, at time
 */

void time_seq_destory_at(struct time_seq_t *time_seq, size_t time)
{
	struct event_queue_head_t *queue_head = time_seq -> queue_array_ptr + time;
	struct event_t *event, *tmp;
	assert(time < time_seq -> length);
	TAILQ_FOREACH_MUTABLE(event, queue_head, queue_entry, tmp)
	{
		TAILQ_REMOVE(queue_head, event, queue_entry);
		free(event);
	}
}
