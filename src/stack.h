#ifndef MY_STACK_
#define MY_STACK_

#include <stdlib.h>

/**
 * @brief Struct representing each element in stack.
 */
typedef struct StackFrame {
  void* data;
  struct StackFrame* next;
} StackFrame;

/**
 * @brief Struct representing stack.
 */
typedef struct Stack {
  StackFrame* head;
  size_t len;
} Stack;

/**
 * @brief Initialize new stack frame.
 *
 * @param data Data to be stored in frame.
 * @return New stack frame.
 */
StackFrame* stack_frame_init(void* data);
/**
 * @brief Free stack frame.
 *
 * @param frame Frame to free.
 * @return Void.
 */
void stack_frame_free(StackFrame* frame);

/**
 * @brief Initialize new stack.
 *
 * @return New stack.
 */
Stack* stack_init();
/**
 * @brief Push data onto stack.
 *
 * @param stack Stack that data will be pushed onto.
 * @param data Data to be added to stack.
 * @return Void.
 */
void stack_push(Stack* stack, void* data);
/**
 * @brief Get topmost value in stack.
 *
 * @param stack Stack to peek at.
 * @return Topmost value in stack.
 */
void* stack_peek(Stack* stack);

/**
 * @brief Remove topmost value from stack.
 *
 * @param stack The target stack.
 * @return Returns 0 if unable to pop else 1.
 */
int stack_pop(Stack* stack);
/**
 * @brief Free stack.
 *
 * @param stack The stack to free.
 * @return Void.
 */
void stack_free(Stack* stack);

#endif /* MY_STACK_ */
