#ifndef __TIME_SEQ_H__
#define __TIME_SEQ_H__
#include<inc/queue.h>
#include<stddef.h>
struct time_seq_t;
struct event_t{
	TAILQ_ENTRY(event_t) queue_entry;
	void (* trigger) (void *self, struct time_seq_t *time_seq, size_t time);
};

TAILQ_HEAD(event_queue_head_t, event_t);

struct time_seq_t{
	struct event_queue_head_t *queue_array_ptr;
	size_t length;
};

void time_seq_destory(struct time_seq_t *time_seq);
struct time_seq_t* time_seq_init(size_t total_millisecond);
inline void time_seq_insert_head(struct time_seq_t *time_seq, size_t time, struct event_t *event);
inline void time_seq_insert_tail(struct time_seq_t *time_seq, size_t time, struct event_t *event);
void time_seq_trigger_at(struct time_seq_t *time_seq, size_t time);
void time_seq_destory_at(struct time_seq_t *time_seq, size_t time);
#endif
