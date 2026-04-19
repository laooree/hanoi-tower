#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NDISKS 5
#define DISK_WIDTH (4+2*(NDISKS-1))
#define TOTAL_WIDTH (3*(DISK_WIDTH+2))
#define FPS 2


////////////////////////////////////
///// IMPLEMENTATION OF STACKS /////
////////////////////////////////////

/* Use stack as data structure:                                     *
 * - every disk has a value and a pointer to the disk below itself; *
 * - a stack contains the pointer to the top and the height.        */
struct disk {
    unsigned long value;
    unsigned long magic;
    struct disk *below;
};

struct stack {
    unsigned long height;
    unsigned long magic;
    struct disk *top;
};


/* Initialize an empty and valid stack. */
struct stack *init_stack(void) {
    struct stack *new_stack = malloc(sizeof(struct stack));
    if (new_stack == NULL) {
        printf("ERROR: malloc() failed to allocate new memory. Aborting.\n");
        exit(1);
    }
    new_stack->height = 0;
    new_stack->magic  = 0xDEADBEEF;
    new_stack->top    = NULL;

    return new_stack;
}


/* Initialize a valid disk. */
struct disk *init_disk(unsigned long value) {
    struct disk *new_disk = malloc(sizeof(struct disk));
    if (new_disk == NULL) {
        printf("ERROR: malloc() failed to allocate new memory. Aborting.\n");
        exit(1);
    }
    new_disk->value = value;
    new_disk->magic = 0xDEADBEEF;
    new_disk->below = NULL;

    return new_disk;
}


/* Check if objects are valid by controlling the magic field. */
void check_stack_validity(struct stack *stack) {
    if (stack == NULL || stack->magic != 0xDEADBEEF){
        printf("ERROR: pointer to an invalid stack found. Aborting.\n");
        exit(1);
    }
    return;
}


void check_disk_validity(struct disk *disk) {
    if (disk == NULL || disk->magic != 0xDEADBEEF){
        printf("ERROR: pointer to an invalid disk found. Aborting.\n");
        exit(1);
    }
    return;
}



/* Push a new disk in the stack, ensuring that disks *
 * are stacked only in decreasing order.             */
void push(struct stack *stack, struct disk *disk) {
    check_stack_validity(stack);
    check_disk_validity(disk);
    if (stack->top != NULL && disk->value >= stack->top->value) {
        // Can't push if disk is bigger than stack's top disk.
        return;
    }

    disk->below = stack->top;
    stack->top  = disk;
    stack->height++;
    return;
}


/* UNSAFE: possible memory leak if the disk is lost after popping. *
 * This function is to be used by move(), and that one should be   *
 * used instead.                                                   */
struct disk *pop(struct stack *stack){
    if (stack->top == NULL) {
        printf("ERROR: popping from an empty stack.\n");
        exit(1);
    }

    struct disk *popped_disk = stack->top;
    stack->top = stack->top->below;
    stack->height--;
    return popped_disk;
}


/* Safe function to pop a disk from a stack and push it to another. *
   Feasibility is checked before popping, to avoid memory leaks.    */
void move(struct stack *from_stack, struct stack *to_stack){
    check_stack_validity(from_stack);
    check_stack_validity(to_stack);

    if (from_stack->top == NULL) {
        // Moving from an empty stack: do nothing.
        return;
    }
    if (to_stack->top != NULL && from_stack->top->value >= to_stack->top->value) {
        // Trying to move bigger disk above smaller one: do nothing.
        return;
    }

    push(to_stack, pop(from_stack));
    return;
}


/* Fill an empty stack with disks from 1 to n */
void fill_empty_stack(struct stack *stack, unsigned long n) {
    check_stack_validity(stack);

    if (stack->top != NULL) {
        // Stack is non-empty: do nothing.
        return;
    }
    for (unsigned long i=n; i>=1; i--) {
        push(stack,init_disk(i));
    }
    return;
}



//////////////////////////////////////////////
///// IMPLEMENTATION OF HANOI TOWER GAME /////
//////////////////////////////////////////////

/* Print a single stack */
void print_stack(struct stack *stack) {
    struct disk *current_disk = stack->top;
    while (current_disk != NULL) {
        printf("< %lu >\n", current_disk->value);
        current_disk = current_disk->below;
    }
    return;
}


void print_disk(struct disk *disk){
    if (disk==NULL) {
        for (int i=1; i<=DISK_WIDTH; i++) {
            if (i == DISK_WIDTH/2 || i == DISK_WIDTH/2+1) {
                putchar('|');
            } else {
                putchar(' ');
            }
        }
    } else {
        for (int i=1; i<=DISK_WIDTH; i++) {
            if (i <= DISK_WIDTH/2 - disk->value -1) {
                putchar(' ');
            } else if (i == DISK_WIDTH/2 - disk->value) {
                putchar('<');
            } else if (i < DISK_WIDTH/2) {
                putchar('-');
            } else if (i == DISK_WIDTH/2) {
                printf("%02lu",disk->value);
            } else if (i == DISK_WIDTH/2 +1) {
                continue;
            } else if (i <= DISK_WIDTH/2 + disk->value) {
                putchar('-');
            } else if (i == DISK_WIDTH/2 + disk->value + 1) {
                putchar('>');
            } else putchar(' ');
        }
    }
    return;
}


/* Print all the three poles */
void print_game(struct stack *sa, struct stack *sb, struct stack *sc) {
    putchar('\n');

    // Iterate through all the levels (number of disks)
    struct disk *disk_a = sa->top;
    struct disk *disk_b = sb->top;
    struct disk *disk_c = sc->top;
    for (int l = NDISKS; l>0; l--) {
        putchar(' ');
        // Print current disk from stack a
        if (sa->height < l) {
            print_disk(NULL);
        } else {
            print_disk(disk_a);
            disk_a = disk_a->below;
        }

        putchar(' '); // separator

        // Print current disk from stack b
        if (sb->height < l) {
            print_disk(NULL);
        } else {
            print_disk(disk_b);
            disk_b = disk_b->below;
        }

        putchar(' '); // separator

        // Print current disk from stack b
        if (sc->height < l) {
            print_disk(NULL);
        } else {
            print_disk(disk_c);
            disk_c = disk_c->below;
        }

        putchar('\n');
    }
}


struct stack *stack_a;
struct stack *stack_b;
struct stack *stack_c;


/* Solve the hanoi tower by moving n disks from source to target, *
 * using aux as an auxiliary staack.                             */
void solve(unsigned long n, struct stack *source, struct stack *target, struct stack *aux) {
    if (n == 0) {
        return;
    } else {
        solve(n-1, source, aux, target);
        move(source, target);
        printf("\x1b[3J\x1b[H\x1b[2J"); // clear screen
        usleep((int)1000000/FPS);
        print_game(stack_a,stack_b,stack_c);
        solve(n-1, aux, target, source);
    }
    return;
}


int main(void) {

    stack_a = init_stack();
    stack_b = init_stack();
    stack_c = init_stack();

    fill_empty_stack(stack_a, NDISKS);

    solve(NDISKS, stack_a, stack_b, stack_c);

    return 0;

}
