#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q != NULL) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q; /* If malloc returned NULL, this function returns NULL */
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* No effect if q is NULL */
    if (q == NULL)
        return;
    /* Free the list elements and the strings */
    while (q->head) {
        list_ele_t *target = q->head;
        q->head = target->next;
        free(target->value);
        free(target);
    }
    /* Free queue structure */
    free(q);
}

static inline bool ele_new(char *s, list_ele_t **newh, char **value)
{
    *newh = malloc(sizeof(list_ele_t));
    *value = strdup(s); /* Allocate space for the string and copy it */

    /* If either call to malloc returns NULL */
    if (*newh == NULL || *value == NULL) {
        if (*newh)
            free(*newh);
        if (*value)
            free(*value);
        return false;
    }
    return true;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL)
        return false;
    list_ele_t *newh;
    char *value;
    if (!ele_new(s, &newh, &value))
        return false;

    /* set up the element and insert at the head of the queue*/
    newh->value = value;
    newh->next = q->head;
    q->head = newh;
    if (q->tail == NULL)
        q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q == NULL)
        return false;
    list_ele_t *newh;
    char *value;
    if (!ele_new(s, &newh, &value))
        return false;

    /* set up the element and insert at the tail of the queue */
    newh->value = value;
    newh->next = NULL;
    if (q->tail != NULL)
        q->tail->next = newh;
    else
        q->head = newh;
    q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL || q->head == NULL)
        return false;
    if (sp != NULL) {
        int length = strlen(q->head->value);
        length = (bufsize - 1 >= length) ? length : bufsize - 1;
        strncpy(sp, q->head->value, length);
        *(sp + length) = '\0';
    }
    list_ele_t *target = q->head;
    if (q->head == q->tail)
        q->head = q->tail = NULL;
    else
        q->head = q->head->next;
    free(target->value);
    free(target);
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return 0;
    else
        return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return;
    q->tail = q->head;
    while (q->tail->next != NULL) {
        list_ele_t *target = q->tail->next;
        q->tail->next = target->next;
        target->next = q->head;
        q->head = target;
    }
}

void frontBackSplit(list_ele_t *source,
                    list_ele_t **frontRef,
                    list_ele_t **backRef)
{
    // if the length is less than 2, handle it separately
    if (source == NULL || source->next == NULL) {
        *frontRef = source;
        *backRef = NULL;
        return;
    }

    list_ele_t *slow = source;
    list_ele_t *fast = source->next;

    // advance `fast` two nodes, and advance `slow` one node
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // `slow` is before the midpoint in the list, so split it in two
    // at that point.
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

list_ele_t *sortedMerge(list_ele_t *a, list_ele_t *b)
{
    list_ele_t *result = NULL, *tail = NULL;

    while (a != NULL && b != NULL) {
        if (strcmp(a->value, b->value) <= 0) {
            if (result == NULL)
                result = a;
            else
                tail->next = a;
            tail = a;
            a = a->next;
        } else {
            if (result == NULL)
                result = b;
            else
                tail->next = b;
            tail = b;
            b = b->next;
        }
        tail->next = NULL;
    }
    tail->next = (a != NULL) ? a : b;

    return result;
}

void mergesort(list_ele_t **head)
{
    // base case — length 0 or 1
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    list_ele_t *a;
    list_ele_t *b;

    // split `head` into `a` and `b` sublists
    frontBackSplit(*head, &a, &b);

    // recursively sort the sublists
    mergesort(&a);
    mergesort(&b);

    // answer = merge the two sorted lists
    *head = sortedMerge(a, b);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return;
    if (q->size == 1) {
        /* no-op */
        return;
    }

    mergesort(&q->head);

    list_ele_t *buf = q->head;
    while (buf) {
        q->tail = buf;
        buf = buf->next;
    }
}
