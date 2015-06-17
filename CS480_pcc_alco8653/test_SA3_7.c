/* Ultimate Typechecking Test Part 1 Alexandra Shinsel*/

int W = 10;
int *X = &W;
int Y[2] = {100, 200};
int *Z[1] = {X};

int foo ()
{
    return 12;
}

/* Initialization */

int a = W;
int b = X;        /* ERROR */
int c = Y;        /* ERROR */
int d = Y[1];
int e = Z;        /* ERROR */
int f = Z[0];    /* ERROR */
int g = foo;
int h = &Y;        /* ERROR */
int i = *X;

int *j = W;
int *k = X;
int *l = Y;
int *m = Y[1];
int *n = Z;
int *o = Z[0];
int *p = foo;
int *q = &Y;
int *r = *X;

                /* All of these should produce errors */
int s[3] = W;
int t[3] = X;
int u[3] = Y;
int v[3] = Y[1];
int w[3] = Z;
int x[3] = Z[0];
int y[3] = foo;
int z[3] = &Y;
int aa[3] = *X;

int *bb[3] = W;
int *cc[3] = X;
int *dd[3] = Y;
int *ee[3] = Y[1];
int *ff[3] = Z;
int *gg[3] = Z[0];
int *hh[3] = foo;
int *ii[3] = &Y;
int *jj[3] = *X;


/* Ultimate Typechecking Testcase Part 2 */


main ()
{
    int a;
    int *b;
    int c[1];
    int *d[1];
    
    a = W;
    a = X;                    /*ERROR*/
    a = *X;
    a = Y;                    /*ERROR*/
    a = Y[0];
    a = Z;                    /*ERROR*/
    a = Z[0];                /*ERROR*/
    a = *Z[0];
    a = foo;

    b = W;
    b = X;
    b = *X;
    b = Y;
    b = Y[0];
    b = Z;
    b = Z[0];
    b = *Z[0];
    b = foo;

            /* All produce errors */
    c = W;
    c = X;
    c = *X;
    c = Y;
    c = Y[0];
    c = Z;
    c = Z[0];
    c = *Z[0];
    c = foo;

    d = W;
    d = X;
    d = *X;
    d = Y;
    d = Y[0];
    d = Z;
    d = Z[0];
    d = *Z[0];
    d = foo;
            /* End error block */
    
    *b = W;
    *b = X;                    /*ERROR*/
    *b = *X;
    *b = Y;                    /*ERROR*/
    *b = Y[0];
    *b = Z;                    /*ERROR*/
    *b = Z[0];            /*ERROR*/
    *b = *Z[0];
    *b = foo;

    c[0] = W;
    c[0] = X;                    /*ERROR*/
    c[0] = *X;
    c[0] = Y;                    /*ERROR*/
    c[0] = Y[0];
    c[0] = Z;                    /*ERROR*/
    c[0] = Z[0];            /*ERROR*/
    c[0] = *Z[0];
    c[0] = foo;

    d[0] = W;
    d[0] = X;
    d[0] = *X;
    d[0] = Y;
    d[0] = Y[0];
    d[0] = Z;
    d[0] = Z[0];
    d[0] = *Z[0];
    d[0] = foo;
}
