#include "fft.h"
/*********************************************************************************
    ????:fft.c
    ????:2018.7.20
    ?        ?:V1.0
    ????:FFT????
    ????:CCS8.1
    ?        ?:?????FFT?????
    ?        ?:???????????,????????????!
    ????:2018.7.20
**********************************************************************************/
/*******************************************************************************
* Function Name : conjugate_complex(int n, complex in[], complex out[])
* Description   : ??????????
* Input         : int n:??????
*                 complex in[]:???????
* Output        : complex out[]:???????
* Return        : None
* tips          :
*******************************************************************************/
void conjugate_complex(int n, complex in[], complex out[])
{
    int i = 0;
    for (i = 0; i<n; i++)
    {
        out[i].imag = -in[i].imag;
        out[i].real = in[i].real;
    }
}
/*******************************************************************************
* Function Name : c_abs(int n, complex f[], float out[])
* Description   : ???????
* Input         : int n:?????????
*                 complex f[]:??????
* Output        : float out[]:???????
* Return        : None
* tips          :
*******************************************************************************/
void c_abs(int n, complex f[], float out[])
{
    int i = 0;
    float t;
    for (i = 0; i<n; i++)
    {
        t = f[i].real * f[i].real + f[i].imag * f[i].imag;
        out[i] = sqrt(t);
    }
}
/*******************************************************************************
* Function Name : c_plus(complex a, complex b, complex *c)
* Description   : ?????
* Input         : complex a:?????a
*                 complex b:?????b
* Output        : complex *c:????????
* Return        : None
* tips          :
*******************************************************************************/
void c_plus(complex a, complex b, complex *c)
{
    c->real = a.real + b.real;
    c->imag = a.imag + b.imag;
}
/*******************************************************************************
* Function Name : c_sub(complex a, complex b, complex *c)
* Description   : ?????
* Input         : complex a:?????a(???)
*                 complex b:?????b(??)
* Output        : complex *c:????????
* Return        : None
* tips          :
*******************************************************************************/
void c_sub(complex a, complex b, complex *c)
{
    c->real = a.real - b.real;
    c->imag = a.imag - b.imag;
}
/*******************************************************************************
* Function Name : c_mul(complex a, complex b, complex *c)
* Description   : ??????
* Input         : complex a:?????a
*                 complex b:?????b
* Output        : complex *c:????????
* Return        : None
* tips          :
*******************************************************************************/
void c_mul(complex a, complex b, complex *c)
{
    c->real = a.real * b.real - a.imag * b.imag;
    c->imag = a.real * b.imag + a.imag * b.real;
}
/*******************************************************************************
* Function Name : c_div(complex a, complex b, complex *c)
* Description   : ??????
* Input         : complex a:?????a(???)
*                 complex b:?????b(??)
* Output        : complex *c:????????
* Return        : None
* tips          :
*******************************************************************************/
void c_div(complex a, complex b, complex *c)
{
    c->real = (a.real * b.real + a.imag * b.imag) / (b.real * b.real + b.imag * b.imag);
    c->imag = (a.imag * b.real - a.real * b.imag) / (b.real * b.real + b.imag * b.imag);
}
/*******************************************************************************
* Function Name : Wn_i(int n, int i, complex *Wn, char flag)
* Description   : ??
* Input         :
*
* Output        :
* Return        : None
* tips          :
*******************************************************************************/
void Wn_i(int n, int i, complex *Wn, char flag)
{
    Wn->real = cos(2 * PI*i / n);
    if (flag == 1)
        Wn->imag = -sin(2 * PI*i / n);
    else if (flag == 0)
        Wn->imag = -sin(2 * PI*i / n);
}
/*******************************************************************************
* Function Name : fft(int N, complex f[])
* Description   : AD?????FFT
* Input         : int Samp_num:AD????
*                 complex f[]:?FFT???(????)
* Output        : complex f[]:?????FFT?????
* Return        : None
* tips          :
*******************************************************************************/
void fft(int Samp_num, complex f[])
{
    complex t, wn;
    int i, j, k, m, n, l, r, M;
    int la, lb, lc;
    /*----???????M=log2(Samp_num)----*/
    for (i = Samp_num, M = 1; (i = i / 2) != 1; M++);
    /*----????????????----*/
    for (i = 1, j = Samp_num / 2; i <= Samp_num - 2; i++)
    {
        if (i<j)
        {
            t = f[j];
            f[j] = f[i];
            f[i] = t;
        }
        k = Samp_num / 2;
        while (k <= j)
        {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }

    /*----FFT??----*/
    for (m = 1; m <= M; m++)
    {
        la = pow(2, (double)(m)); //la=2^m???m??????????
        lb = la / 2;    //lb???m????????????
        //?????????????????????
        /*----????----*/
        for (l = 1; l <= lb; l++)
        {
            r = (l - 1)*pow(2, (double)(M - m));
            for (n = l - 1; n<Samp_num - 1; n = n + la) //??????,?????N/la
            {
                lc = n + lb;  //n,lc??????????????????
                Wn_i(Samp_num, r, &wn, 1);//wn=Wnr
                c_mul(f[lc], wn, &t);//t = f[lc] * wn????
                c_sub(f[n], t, &(f[lc]));//f[lc] = f[n] - f[lc] * Wnr
                c_plus(f[n], t, &(f[n]));//f[n] = f[n] + f[lc] * Wnr
            }
        }
    }
}
/*******************************************************************************
* Function Name : ifft(int N, complex f[])
* Description   : FFT??????
* Input         : int Samp_num:AD????
*                 complex f[]:?ifft???
* Output        : complex f[]:?????ifft?????(????)
* Return        : None
* tips          :
*******************************************************************************/
void ifft(int Samp_num, complex f[])
{
    int i = 0;
    conjugate_complex(Samp_num, f, f);
    fft(Samp_num, f);
    conjugate_complex(Samp_num, f, f);
    for (i = 0; i<Samp_num; i++)
    {
        f[i].imag = (f[i].imag) / Samp_num;
        f[i].real = (f[i].real) / Samp_num;
    }
}
