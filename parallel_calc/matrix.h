#ifndef _MATRIX_H_
#define _MATRIX_H_

enum ret_value {
    FAIL = -1,
    SUCCESS
};

typedef struct {
    double** m;
    size_t size;
    double det;
} matrix;

int m_create    (matrix* m, size_t size);
int m_clear     (matrix* m);
int m_read      (matrix* m, int fd);
int m_print     (matrix* m);
int m_determ    (matrix* m, double* value);
int m_copy      (matrix* dst, matrix* src);

#endif
