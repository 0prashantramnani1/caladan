/*
 * poll.h - support for event polling (similar to select/epoll/poll, etc.)
 */

#include <runtime/poll.h>
#include <runtime/smalloc.h>
#include "net/mbufq.h"

/**
 * poll_init - initializes a polling waiter object
 * @w: the waiter object to initialize
 */
void poll_init(poll_waiter_t *w)
{
	spin_lock_init(&w->lock);
	list_head_init(&w->triggered);
	w->waiting_th = NULL;
	w->counter = 0;
}

/**
 * create_waiter - allocate memory for waiter and initialize it
 * @w_out: a pointer to store the waiter (if successful)
 */
int create_waiter(poll_waiter_t **w_out)
{
	poll_waiter_t *w;
	w = smalloc(sizeof(*w));
	if (!w)
		return -ENOMEM;
	poll_init(w);
	*w_out = w;
	return 0;
}

/**
 * create_trigger - allocate memory for trigger
 * @t_out: a pointer to store the trigger (if successful)
 */
int create_trigger(poll_trigger_t **t_out)
{
	poll_trigger_t *t;
	t = smalloc(sizeof(*t));
	if (!t)
		return -ENOMEM;

	poll_trigger_init(t);
	*t_out = t;
	return 0;
}

/**
 * poll_arm - registers a trigger with a waiter
 * @w: the waiter to register with
 * @t: the trigger to register
 * @data: data to provide when the trigger fires
 */
void poll_arm(poll_waiter_t *w, poll_trigger_t *t, unsigned long data)
{
	if (WARN_ON(t->waiter != NULL))
		return;

	t->waiter = w;
	t->triggered = false;
	t->data = data;
	
	spin_lock_np(&w->lock);
	w->counter++;
	spin_unlock_np(&w->lock);
}

/**
 * poll_arm_w_sock -  register a trigger with a waiter and a socket
 * @w: the waiter to register with
 * @sock_event_head: the list head of triggers associated with the socket
 * @t: the trigger to register
 * @cb: the callback called with the trigger fires
 * @cb_arg: the argument passed to the callback
 */
void poll_arm_w_sock(poll_waiter_t *w, struct list_head *sock_event_head,
	poll_trigger_t *t, short event_type, sh_event_callback_fn cb,
	void* cb_arg, tcpconn_t *sock, unsigned long data) {
	if (WARN_ON(t->waiter != NULL))
		return;

	t->waiter = w;
	t->triggered = false;
	t->event_type = event_type;
	t->cb = cb;
	t->cb_arg = cb_arg;
	t->sock = sock;
	t->data = data;
	if(sock_event_head == NULL) {
		//printf("poll_wait: Null check 1\n");
	}

	list_add(sock_event_head, &t->sock_link);

	spin_lock_np(&w->lock);
	w->counter++;
	spin_unlock_np(&w->lock);
}

void poll_arm_w_queue(poll_waiter_t *w, struct list_head *sock_event_head,
	poll_trigger_t *t, short event_type, sh_event_callback_fn cb,
	void* cb_arg, tcpqueue_t *queue, unsigned long data) {
	if (WARN_ON(t->waiter != NULL))
		return;

	t->waiter = w;
	t->triggered = false;
	t->event_type = event_type;
	t->cb = cb;
	t->cb_arg = cb_arg;
	t->queue = queue;
	t->data = data;
	if(sock_event_head == NULL) {
		//printf("poll_wait: Null check 1\n");
	}

	list_add(sock_event_head, &t->sock_link);

	spin_lock_np(&w->lock);
	w->counter++;
	spin_unlock_np(&w->lock);
}

// TODO: Make it general

void poll_arm_w_sock_neper(poll_waiter_t *w, struct list_head *sock_event_head,
	poll_trigger_t *t, short event_type, sh_event_callback_fn cb,
	void* cb_arg, tcpconn_t *sock, void* data) {
	if (WARN_ON(t->waiter != NULL))
		return;

	t->waiter = w;
	t->triggered = false;
	t->event_type = event_type;
	t->cb = cb;
	t->cb_arg = cb_arg;
	t->sock = sock;
	t->data_poll = data;

	if(sock_event_head != NULL)
		list_add(sock_event_head, &t->sock_link);

	spin_lock_np(&w->lock);

	// Hack to manually set write epoll true
	if(event_type & SEV_WRITE) {
		t->triggered = true;
		list_add(&w->triggered, &t->link);
	}
	
	w->counter++;
	spin_unlock_np(&w->lock);
}

void poll_arm_w_queue_neper(poll_waiter_t *w, struct list_head *sock_event_head,
	poll_trigger_t *t, short event_type, sh_event_callback_fn cb,
	void* cb_arg, tcpqueue_t *queue, void* data) {
	if (WARN_ON(t->waiter != NULL))
		return;

	t->waiter = w;
	t->triggered = false;
	t->event_type = event_type;
	t->cb = cb;
	t->cb_arg = cb_arg;
	t->queue = queue;
	t->data_poll = data;

	list_add(sock_event_head, &t->sock_link);

	spin_lock_np(&w->lock);
	w->counter++;
	spin_unlock_np(&w->lock);
}
/**
 * poll_disarm - unregisters a trigger with a waiter
 * @t: the trigger to unregister
 */
void poll_disarm(poll_trigger_t *t)
{
	poll_waiter_t *w;
	if (WARN_ON(t->waiter == NULL))
		return;

	w = t->waiter;
	w->counter--;
	spin_lock_np(&w->lock);
	if (t->triggered) {
		list_del(&t->link);
		t->triggered = false;
	}
	spin_unlock_np(&w->lock);

	t->waiter = NULL;
}

/**
 * poll_wait - waits for the next event to trigger
 * @w: the waiter to wait on
 *
 * Returns the data provided to the trigger that fired
 */
unsigned long poll_wait(poll_waiter_t *w)
{
	//printf("poll wait: \n");
	thread_t *th = thread_self();
	poll_trigger_t *t;

	while (true) {
		//printf("poll_wait: in while loop \n");
		spin_lock_np(&w->lock);
		t = list_pop(&w->triggered, poll_trigger_t, link);
		if (t) {
			//printf("poll_wait: trigger triggered\n");
			spin_unlock_np(&w->lock);
			return t->data;
		}
		w->waiting_th = th;
		thread_park_and_unlock_np(&w->lock);
	}
}

/**
 * poll_cb_once - loops over all triggered events and calls their callbacks
 * @w: the waiter to wait on
 */
int poll_cb_once_nonblock(poll_waiter_t *w)
{
	poll_trigger_t *t;
	int ret = 1;

	/* (@saubhik): If no event is triggered, return.
	 * If a triggered event is found, then process it,
	 * and process all triggered events, and return.
	 */
	while (true) {

		spin_lock_np(&w->lock);

		if (!w->counter) {
			spin_unlock_np(&w->lock);
			return -1;
		}

		t = list_pop(&w->triggered, poll_trigger_t, link);

		if (!t) {
			spin_unlock_np(&w->lock);
			return ret;
		}

		t->triggered = false;
		spin_unlock_np(&w->lock);

		ret = 0;
	}
}

/**
 * poll_cb_once - blocks till an event is triggered and loops over all
 * triggered events and calls their callbacks
 * @w: the waiter to wait on
 */
int poll_cb_once(poll_waiter_t *w)
{
	bool processed_events = false;
	poll_trigger_t *t;
	int ret = 1;

	while (true) {
		spin_lock_np(&w->lock);
		if (!w->counter) {
			spin_unlock_np(&w->lock);
			return -1;
		}

		t = list_pop(&w->triggered, poll_trigger_t, link);

		if (!t) {
			spin_unlock_np(&w->lock);
			if (processed_events) return 0;
			return ret;
		}

		t->triggered = false;
		spin_unlock_np(&w->lock);
		if(t->cb != NULL)
			t->cb(t->cb_arg);
		tcparg_t *arg = (tcparg_t *)t->cb_arg;
		// printf("poll_cb_once: called tcp_accep_poll: raddr_port:%d\n", tcp_get_raddr_port(*arg->c));
		processed_events = true;

		if(t->queue != NULL) {
			tcpqueue_check_triggers(t->queue);
			return 0;
		}
		ret = 0;
	}
}

/**
 * poll_cb_once - blocks till an event is triggered and loops over all
 * triggered events and calls their callbacks
 * @w: the waiter to wait on
 */
int poll_return_triggers(poll_waiter_t *w, poll_trigger_t **events, int max_events)
{
	poll_trigger_t *t;
	int ret = 1;
	int nevents = 0;

	spin_lock_np(&w->lock);
	while (true) {
		if (!w->counter) {
			spin_unlock_np(&w->lock);
			return -1;
		}

		t = list_pop(&w->triggered, poll_trigger_t, link);

		if (!t) {
			
			// Hack to set the write triggers again
			for(int i=0;i<nevents;i++) {
				if(events[i]->event_type & SEV_WRITE) {
					events[i]->triggered = true;
					list_add(&w->triggered, &(events[i]->link));
				}
			}

			spin_unlock_np(&w->lock);
			return nevents;
		}

		t->triggered = false;

		events[nevents] = t;
		nevents++;
		if(nevents >= max_events) {

			for(int i=0;i<nevents;i++) {
				if(events[i]->event_type & SEV_WRITE) {
					events[i]->triggered = true;
					list_add(&w->triggered, &(events[i]->link));
				}
			}

			spin_unlock_np(&w->lock);
			return nevents;
		}
	}
}


/**
 * poll_trigger - fires a trigger
 * @w: the waiter to wake up (if it is waiting)
 * @t: the trigger that fired
 */
void poll_trigger(poll_waiter_t *w, poll_trigger_t *t)
{
	thread_t *wth = NULL;

	spin_lock_np(&w->lock);
	if (t->triggered) {
		spin_unlock_np(&w->lock);
		return;
	}

	t->triggered = true;

	list_add(&w->triggered, &t->link);
	if (w->waiting_th) {
		wth = w->waiting_th;
		w->waiting_th = NULL;
	}
	spin_unlock_np(&w->lock);

	if (wth)
		thread_ready(wth);
}
