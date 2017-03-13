#ifndef MY_STACK_
#define MY_STACK_

#include <stdlib.h>

/**
 * @struct StackFrame
 * @brief Struct representing each element in stack.
 */
typedef struct StackFrame {
  void* data;
  struct StackFrame* next;
} StackFrame;

/**
 * @struct Stack
 * @brief Struct representing stack.
 */
typedef struct Stack {
  StackFrame* head;
  size_t len;
} Stack;

////////////////////////////////////////////////////////////////////////////////
/// STACK FRAME
////////////////////////////////////////////////////////////////////////////////
StackFrame* stack_frame_init(void* data);
void stack_frame_free(StackFrame** frame);

////////////////////////////////////////////////////////////////////////////////
/// STACK
////////////////////////////////////////////////////////////////////////////////
Stack* stack_init();
void stack_push(Stack* stack, void* data);
void* stack_peek(Stack* stack);
int stack_pop(Stack* stack);
void* stack_pop_return(Stack* stack);
void stack_free(Stack** stack);

#endif /* MY_STACK_ */
