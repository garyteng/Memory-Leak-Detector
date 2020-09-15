/**
 * Mini Valgrind Lab
 * CS 241 - Fall 2016
 */

#include "mini_valgrind.h"
#include "print.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef malloc
#undef realloc
#undef free


/*
 * Replace normal malloc, this malloc will also create meta data info
 * and insert it to the head of the list.
 * You have to malloc enough size to hold both the size of your allocated
 * and the meta_data structure.
 * In this function, you should only call malloc only once.
 *
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 *
 * @return
 *	On success, return a pointer to the memory block allocated by
 *	the function. Note that this pointer should return the actual
 *	address the pointer starts and not the meta_data.
 *
 *	If the function fails to allocate the requested block of memory,
 *	return a null pointer.
 */
void *mini_malloc(size_t size, const char *file, size_t line) {
  
    total_usage += size;

    void *result = (void*) malloc( sizeof(meta_data) + size );
    insert_meta_data(result, size, file, line);
  
    return (result+sizeof(meta_data));
}

/*
 * Replace normal realloc, this realloc will also first check whether the
 * pointer that passed in has memory. If it has memory then resize the memory
 * to it. Or if the pointer doesn't have any memory, then call malloc to
 * provide memory.
 * For total usage calculation, if the new size is larger than the old size,
 * the total usage should increase the difference between the old size and the
 * new size. If the new size is smeller or equal to the old size, the total
 * usage should remain the same.
 * You have to realloc enough size to hold both the size of your allocated
 * and the meta_data structure.
 * In this function, you should only call malloc only once.
 *
 * @param ptr
 *      The pointer require realloc
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 *
 * @return
 *	On success, return a pointer to the memory block allocated by
 *	the function. Note that this pointer should return the actual
 *	address the pointer starts and not the meta_data.
 *
 *	If the function fails to allocate the requested block of memory,
 *	return a null pointer.
 */
void *mini_realloc(void *ptr, size_t size, const char *file, size_t line) {

    if(ptr == NULL){
        return mini_malloc(size, file, line);
    }

    meta_data *old_data = ( ptr - sizeof(meta_data) );

    if(old_data->size >= size){ // Nothing needs to be changed
        return ptr;
    }else{

        void *new_ptr = mini_malloc(size, file, line);
        size_t copy_size = ( size <= old_data->size ? size : old_data->size);
        memcpy(new_ptr, ptr, copy_size);
        remove_meta_data(old_data);

        return new_ptr;
    }

}

/*
 * Replace normal free, this free will also delete the node in the list.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated. If a null pointer is
 *	passed, no action occurs.
 */
void mini_free(void *ptr) {

    if(ptr == NULL){
        bad_frees++;
        return;
    }

    remove_meta_data( ptr - sizeof(meta_data) );
}

/*
 * Helper function to insert the malloc ptr node to the list.
 * Accumulate total_usage here.
 *
 * @param md
 * 	Pointer to the meta_data
 * @param size
 *	Size of the memory block, in bytes.
 * @param file
 *	Name of the file that uses mini_valgrind to detect memory leak.
 * @param line
 *	Line in the file that causes memory leak
 */
void insert_meta_data(meta_data *md, size_t size, 
                      const char *file, size_t line) {

    /* set value for malloc_info*/
    md->size = size;
    strcpy(md->file_name, file);
    md->line_num = line;
    md->next = head;
    head = md;
}

/*
 * Helper function to remove the free ptr node from the list.
 * Add to total_free here.
 *
 * @param ptr
 *	Pointer to a memory block previously allocated.
 */
void remove_meta_data(void *ptr) {
  /* check if ptr is in the list and delete it from list */

    meta_data *search = head;
    meta_data *my_ptr = ptr;

    if(head == NULL){
        bad_frees++;
        return;
    }else if( head == my_ptr ){
        total_free += my_ptr->size;

        meta_data *new_head = head->next;
        free(head);
        head = new_head;
        return;
    }

    // Start to search
    while(search->next != NULL && search->next != my_ptr){
        search = search->next;
    }

    if(search->next != NULL){
        total_free += my_ptr->size;

        void *temp = search->next;
        search->next = search->next->next;
        free(temp);
    }else if(search->next == NULL){ // Cannot find
        bad_frees++;
    }

}

/*
 * Deletes all nodes from the list. Called when the program exits so make sure
 * to not count these blocks as freed.
 */
void destroy() {

    meta_data *next = head;

    while(head != NULL){
        next = head->next;
        free( (void*) head);
        head = next;
    }

}// Destroy

/*
 * Print mini_valgrind leak report.
 */
void print_report() {
    print_leak_info(head, total_usage, total_free, bad_frees);
}
