#include <stdlib.h>
#include "stack.h"

/**
 * @brief Initialize new stack frame.
 *
 * @param data Data to be stored in frame.
 * @return New stack frame.
 */
StackFrame*
stack_frame_init(void* data)
{
  StackFrame* frame = malloc(sizeof(StackFrame));
  frame->data = data;
  frame->next = NULL;
  return frame;
}
/**
 * @brief Free stack frame.
 *
 * @param frame Frame to free.
 * @return Void.
 */
void
stack_frame_free(StackFrame** frame)
{
  free(*frame);
  *frame = NULL;
}

/**
 * @brief Initialize new stack.
 *
 * @return New stack.
 */
Stack*
stack_init()
{
  Stack* stack = malloc(sizeof(Stack));
  stack->head = NULL;
  stack->len = 0;
  return stack;
}
/**
 * @brief Push data onto stack.
 *
 * @param stack Stack that data will be pushed onto.
 * @param data Data to be added to stack.
 * @return Void.
 */
void
stack_push(Stack* stack, void* data)
{
  if (stack->len == 0) {
    stack->head = stack_frame_init(data);
  } else {
    StackFrame* next = stack_frame_init(data);
    next->next = stack->head;
    stack->head = next;
  }
  stack->len++;
}
/**
 * @brief Get topmost value in stack without removing it.
 *
 * @param stack Stack to peek at.
 * @return Topmost value in stack.
 */
void*
stack_peek(Stack* stack)
{
  return stack->head->data;
}
/**
 * @brief Remove topmost value from stack.
 *
 * @param stack Target stack.
 * @return Returns 0 if unable to pop else 1.
 */
int
stack_pop(Stack* stack)
{
  if (stack->len == 0) {
    return 0;
  } else {
    StackFrame* old_head = stack->head;
    stack->head = stack->head->next;
    stack_frame_free(&old_head);
    stack->len--;
    return 1;
  }
}
/**
 * @brief Remove topmost value from stack and return it.
 *
 * @param stack Target stack.
 * @return Previous top of stack.
 */
void*
stack_pop_return(Stack* stack)
{
  if (stack->len == 0) {
    return NULL;
  } else {
    StackFrame* old_head = stack->head;
    void* old_data = stack_peek(stack);
    stack->head = stack->head->next;
    stack_frame_free(&old_head);
    stack->len--;
    return old_data;
  }
}
/**
 * @brief Free stack and its stack frames.
 *
 * NOTE: Does not free data stored in stack frames. Data is assumed to be
 * stored in memory stack.
 *
 * @param stack Target stack.
 * @return Void.
 */
void
stack_free(Stack** stack)
{
  while (stack_pop(*stack) != 0) {
    continue;
  }
  free(*stack);
  *stack = NULL;
}

