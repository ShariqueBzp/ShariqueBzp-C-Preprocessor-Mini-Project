#ifndef MATHX_H
#define MATHX_H

#define PI            3.14159265358979
#define CIRCLE_AREA(r)  (PI * (r) * (r))
#define MIN(a, b)      ((a) < (b) ? (a) : (b))
#define MAX2(a, b)     ((a) > (b) ? (a) : (b))

double circle_double_area(double r);
int min_int(int a, int b);

#endif
