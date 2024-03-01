#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head)) {
        free(head);
        return;
    }
    element_t *del, *safe;
    list_for_each_entry_safe (del, safe, head, list) {
        list_del(&del->list);
        free(del->value);
        free(del);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));

    char *s1 = malloc(strlen(s) + 1);
    if (!s1 || !ele) {
        free(s1);
        free(ele);
        return false;
    }
    strncpy(s1, s, strlen(s) + 1);
    ele->value = s1;
    struct list_head *new = &ele->list;
    list_add(new, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));

    char *s1 = malloc(strlen(s) + 1);
    if (!s1 || !ele) {
        free(s1);
        free(ele);
        return false;
    }
    strncpy(s1, s, strlen(s) + 1);
    ele->value = s1;
    struct list_head *new = &ele->list;
    list_add_tail(new, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);
    list_del_init(&node->list);
    size_t len = strlen(node->value);
    int i = 0;
    while (bufsize && len) {
        *(sp + i) = *(node->value + i);
        i++;
        bufsize--;
        len--;
    }
    if (bufsize == 0) {
        i--;
        *(sp + i) = '\0';
    } else
        *(sp + i) = '\0';

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    list_del_init(&node->list);
    size_t len = strlen(node->value);
    int i = 0;
    while (bufsize && len) {
        *(sp + i) = *(node->value + i);
        i++;
        bufsize--;
        len--;
    }
    if (bufsize == 0) {
        i--;
        *(sp + i) = '\0';
    } else
        *(sp + i) = '\0';

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || head->next == head)
        return false;
    struct list_head **indir = &head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;
    struct list_head *tmp = *indir;
    list_del(tmp);
    element_t *del = list_entry(tmp, element_t, list);
    free(del->value);
    free(del);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    if (list_empty(head))
        return true;
    struct list_head *slow = head->next;
    for (struct list_head *fast = slow->next; fast != head; fast = fast->next) {
        int cmp = strcmp(list_entry(fast, element_t, list)->value,
                         list_entry(slow, element_t, list)->value);
        if (cmp == 0) {
            struct list_head *del;
            while (fast != head &&
                   !strcmp(list_entry(fast, element_t, list)->value,
                           list_entry(slow, element_t, list)->value)) {
                del = fast;
                fast = fast->next;
                list_del(del);
                q_release_element(list_entry(del, element_t, list));
            }
            del = slow;
            slow = slow->next;
            list_del(del);
            q_release_element(list_entry(del, element_t, list));
            if (fast == head)
                break;
        } else
            slow = slow->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *tmp = head->next;
    struct list_head *tmp2 = NULL;
    while (tmp != head && tmp->next != head) {
        tmp->next->next->prev = tmp->next->prev;
        tmp->next->prev = tmp->prev;
        tmp->prev->next = tmp->next;
        tmp->prev = tmp->next;
        tmp2 = tmp->next;
        tmp->next = tmp->next->next;
        tmp2->next = tmp;
        tmp = tmp->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *tmp = head;
    struct list_head *change = NULL;
    do {
        change = tmp->prev;
        tmp->prev = tmp->next;
        tmp->next = change;
        tmp = tmp->prev;
    } while (tmp != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *mergeList(struct list_head *head1,
                            struct list_head *head2,
                            bool descend)
{
    struct list_head *newhead = NULL, **ptr = &newhead, **node, *prev1 = NULL;

    for (node = NULL; head1 && head2; *node = (*node)->next) {
        element_t *ele1 = list_entry(head1, element_t, list);
        element_t *ele2 = list_entry(head2, element_t, list);
        int cmp = strcmp(ele1->value, ele2->value);
        if (descend)
            node = (cmp >= 0) ? &head1 : &head2;
        else
            node = (cmp <= 0) ? &head1 : &head2;
        *ptr = *node;
        (*ptr)->prev = prev1;
        prev1 = *ptr;
        ptr = &(*ptr)->next;
    }
    if (!head1)
        *ptr = head2;
    else
        *ptr = head1;
    (*ptr)->prev = prev1;
    return newhead;
}

struct list_head *mergeSort(struct list_head *head1, bool descend)
{
    if (!head1 || !head1->next)
        return head1;
    struct list_head *fast = head1->next;
    struct list_head *slow = head1;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    struct list_head *l1 = mergeSort(head1, descend);
    struct list_head *l2 = mergeSort(fast, descend);
    return mergeList(l1, l2, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;
    struct list_head *l1 = mergeSort(head->next, descend);
    head->next = l1;
    l1->prev = head;
    while (l1->next)
        l1 = l1->next;
    head->prev = l1;
    l1->next = head;
}

void removeHelper(struct list_head *head, bool descend, struct list_head *tail)
{
    if (head->next == tail)
        return;
    removeHelper(head->next, descend, tail);
    int cmp = strcmp(list_entry(head, element_t, list)->value,
                     list_entry(head->next, element_t, list)->value);
    if (descend) {
        if (cmp < 0) {
            list_del(head);
            q_release_element(list_entry(head, element_t, list));
        }
    } else {
        if (cmp > 0) {
            list_del(head);
            q_release_element(list_entry(head, element_t, list));
        }
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    removeHelper(head->next, false, head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    removeHelper(head->next, true, head);
    return q_size(head);
}



/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    return 0;
}
