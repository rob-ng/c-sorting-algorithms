/** 
 * @file
 * @brief Stack header file.
 */
#ifndef MY_STACK_
#define MY_STACK_

#include <stdlib.h>

/**
 * @ingroup StackFrame
 * @struct StackFrame
 * @brief Struct to represent a frame in a stack.
 */
typedef struct StackFrame {
  void* data; ///< Data stored in stack frame.
  struct StackFrame* next; ///< Next frame in stack.
} StackFrame;

/**
 * @ingroup Stack
 * @struct Stack
 * @brief Struct to represent a stack.
 */
typedef struct Stack {
  StackFrame* head; ///< Top frame in stack.
  size_t len; ///< Total number of frames in stack.
} Stack;

//##############################################################################
//# STACK FRAME
//##############################################################################

StackFrame* stack_frame_init(void* data);
void stack_frame_free(StackFrame** frame);

//##############################################################################
//# STACK
//##############################################################################

Stack* stack_init();
void stack_push(Stack* stack, void* data);
void* stack_peek(Stack* stack);
int stack_pop(Stack* stack);
void* stack_pop_return(Stack* stack);
void stack_free(Stack** stack);

#endif /* MY_STACK_ */
