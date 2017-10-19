#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include "matrix.h"

int m_create (matrix* m, size_t size) {
    
    if (m == NULL) {
        errno = EINVAL;
        return FAIL;
    }

    m->m = (double**) calloc (size, sizeof (double*));
    if (m->m == NULL)
        return FAIL;
    m->size = size;
    
    int i = 0;
    int j = 0;
    for (i = 0; i < size; i++) {
        m->m[i] = (double*) calloc (size, sizeof (double));
        if (m->m == NULL) {
            for (j = 0; j < i; j++) 
                free (m->m[j]);
            free (m->m);
            return FAIL;
        }
    }
    m->det = 1.0;

    return SUCCESS;
}

int m_clear (matrix* m) {
    
    if (m == NULL) {
        errno = EINVAL;
        return FAIL;
    }
    
    int i = 0;
    for (i = 0; i < m->size; i++) 
        free (m->m[i]);
    free (m->m);

    return SUCCESS;
}

int m_read (matrix* m, int fd) {

    if (fd < 0 || m == NULL) {
        errno = EINVAL;
        return FAIL;
    }

	FILE* file = fdopen (fd, "r");

	if (file == NULL) 
        return FAIL;

    int i = 0;
    int j = 0;
    for (i = 0; i < m->size; i++) {
        for (j = 0; j < m->size; j++) {
            fscanf (file, "%lf", &m->m[i][j]);
        }
    }

    //fclose (file);

    return SUCCESS;
}

int m_print (matrix* m) {

    if (m == NULL) {
        errno = EINVAL;
        return FAIL;
    }

     
    int i = 0;
    int j = 0;
    for (i = 0; i < m->size; i++) {
        for (j = 0; j < m->size; j++) {
            printf ("%f ", m->m[i][j]);
        }
        printf ("\n");
    }
    printf ("\n");

    return SUCCESS;
}

int m_copy (matrix* dst, matrix* src) {

    if (dst == NULL || src == NULL || dst->size != src->size) {
        errno = EINVAL;
        return FAIL;
    }
    
    int i = 0;
    for (i = 0; i < dst->size; i++) 
        memcpy (dst->m[i], src->m[i], dst->size * sizeof (double));
    
    return SUCCESS;
}

int m_determ (matrix* m, double* value) {
    
    if (m == NULL) {
        errno = EINVAL;
        return FAIL;
    }

    int i = 0;
    *value = 1;
    for (i = 0; i < m->size; i++)
        *value *= m->m[i][i];
    
    return SUCCESS;
}
