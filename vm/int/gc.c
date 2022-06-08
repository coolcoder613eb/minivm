#include "gc.h"

void vm_gc_init(vm_gc_t *restrict out)
{
    out->arr_buf = NULL;
    out->arr_lens = NULL;
    out->arr_marks = NULL;
    out->arr_used = 0;
    out->arr_alloc = 0;

    out->move_buf = 0;
    out->move_alloc = 0;

    out->count = 0;
    out->max = VM_GC_MIN;
    out->running = false;
}

void vm_gc_stop(vm_gc_t gc)
{
    for (vm_int_t i = 0; i < gc.arr_used; i++) {
        vm_free(gc.arr_buf[i]);
    }
    vm_free(gc.arr_buf);
    vm_free(gc.arr_lens);
    vm_free(gc.arr_marks);
}

void vm_gc_mark(vm_gc_t *restrict gc, vm_value_t val)
{
    if (VM_VALUE_IS_ARR(val))
    {
        vm_int_t nth = VM_VALUE_GET_ARR(val);
        if (nth < gc->arr_used)
        {
            if (gc->arr_marks[nth] != 1)
            {
                gc->arr_marks[nth] = 1;
                vm_int_t len = gc->arr_lens[nth];
                for (vm_int_t i = 0; i < len; i++)
                {
                    vm_gc_mark(gc, gc->arr_buf[nth][i]);
                }
            }
        }
    }
}

void vm_gc_move_arr(vm_gc_t *restrict gc, vm_value_t *set)
{
    if (VM_VALUE_IS_ARR(*set))
    {
        vm_int_t nth = VM_VALUE_GET_ARR(*set);
        if (nth < gc->arr_used && gc->arr_marks[nth] != 0)
        {
            gc->arr_marks[nth] = 0;
            vm_int_t newpos = gc->move_buf[nth];
            *set = VM_VALUE_SET_ARR(newpos);
            vm_int_t len = gc->arr_lens[newpos];
            for (vm_int_t i = 0; i < len; i++)
            {
                vm_gc_move_arr(gc, &gc->arr_buf[newpos][i]);
            }
        }
    }
}

void vm_gc_run(vm_gc_t *restrict gc)
{
    if (gc->count < gc->max) {
        return;
    }
    for (vm_int_t i = 0; i < gc->nregs; i++)
    {
        vm_gc_mark(gc, gc->regs[i]);
    }
    {
        if (gc->arr_alloc > gc->move_alloc)
        {
            gc->move_alloc = gc->arr_alloc * 2;
            gc->move_buf = vm_realloc(gc->move_buf, sizeof(vm_int_t) * gc->move_alloc);
        }
        vm_int_t arr_used = 0;
        for (vm_int_t i = 0; i < gc->arr_used; i++)
        {
            if (gc->arr_marks[i] == 1)
            {
                {
                    vm_value_t *tmp = gc->arr_buf[arr_used];
                    gc->arr_buf[arr_used] = gc->arr_buf[i];
                    gc->arr_buf[i] = tmp;
                }
                {
                    vm_int_t tmp = gc->arr_lens[arr_used];
                    gc->arr_lens[arr_used] = gc->arr_lens[i];
                    gc->arr_lens[i] = tmp;
                }
                gc->move_buf[i] = arr_used;
                arr_used += 1;
            }
            else
            {
                vm_free(gc->arr_buf[i]);
            }
        }
        for (vm_int_t i = 0; i < gc->nregs; i++)
        {
            vm_gc_move_arr(gc, &gc->regs[i]);
        }
        gc->arr_used = arr_used;
    }
    gc->count = 0;
    gc->max = gc->arr_used;
    if (gc->max < VM_GC_MIN)
    {
        gc->max = VM_GC_MIN;
    }
}

vm_int_t vm_gc_arr(vm_gc_t *restrict gc, vm_int_t size)
{
    vm_int_t next_head = gc->arr_used + 1;
    if (gc->arr_alloc <= next_head)
    {
        vm_int_t next_alloc = next_head * 2;
        gc->arr_buf = vm_realloc(gc->arr_buf, sizeof(vm_value_t *) * next_alloc);
        gc->arr_lens = vm_realloc(gc->arr_lens, sizeof(vm_int_t) * next_alloc);
        gc->arr_marks = vm_realloc(gc->arr_marks, sizeof(uint8_t) * next_alloc);
        for (vm_int_t i = gc->arr_alloc; i < next_alloc; i++)
        {
            gc->arr_buf[i] = NULL;
            gc->arr_marks[i] = 0;
        }
        gc->arr_alloc = next_alloc;
    }
    vm_int_t ret = gc->arr_used;
    gc->arr_lens[ret] = (vm_int_t)size;
    gc->arr_buf[ret] = vm_malloc(sizeof(vm_value_t) * size);
    gc->arr_used = next_head;
    gc->count += 1;
    return ret;
}
