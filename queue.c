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

/*
 * Sort elements of linked list in ascending order using recursive radix sort
 * @param head - a pointer to the pointer to head of the list
 * @param tail - a pointer to the pointer to tail of the list
 * @param index - the index of character in string now checking
 */
void q_ele_radixsort(list_ele_t **head, list_ele_t **tail, int index)
{
    if (*head == *tail)
        return;

    list_ele_t *bucket_head[27], *bucket_tail[27];
    list_ele_t *target = *head;
    for (int iter = 0; iter < 27; ++iter)
        bucket_head[iter] = bucket_tail[iter] = NULL;
    
    while (target != NULL) {
        int charCode = (int)(target->value[index]);
        charCode = (charCode == 0) ? 0 : charCode - 'a' + 1;
        if (bucket_head[charCode] == NULL)
            bucket_head[charCode] = target;
        else
            bucket_tail[charCode]->next = target;
        bucket_tail[charCode] = target;
        target = target->next;
        bucket_tail[charCode]->next = NULL;
    }

    for (int iter = 1; iter < 27; ++iter) {
        if (bucket_head[iter] != bucket_tail[iter])
            q_ele_radixsort(&bucket_head[iter], &bucket_tail[iter], index + 1);
    }

    bool isHead = true;
    for (int iter = 0; iter < 27; ++iter) {
        if (bucket_head[iter] != NULL) {
            if (isHead) {
                *head = bucket_head[iter];
                isHead = false;
            } else {
                (*tail)->next = bucket_head[iter];
            }
            *tail = bucket_tail[iter];
        }
    }

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
    q_ele_radixsort(&(q->head), &(q->tail), 0);
}
