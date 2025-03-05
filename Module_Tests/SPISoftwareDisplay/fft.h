/*********************************************************************************
    ????:fft.h
    ????:2018.7.20
    ?        ?:V1.0
    ????:FFT???
    ????:CCS8.1
    ?        ?:?????FFT????????????
    ?        ?:???????????,????????????!
    ????:2018.7.20
**********************************************************************************/
#include <math.h>
#ifndef FFT_H_
#define FFT_H_


#endif /* FFT_H_ */

typedef struct complex
{
    float real;
    float imag;

}complex;
#define PI 3.14151926535897935384626433832795       //pi
#define FFT_NPT 4096		//FFT????
#define Samp_Fs	2048		//????

void conjugate_complex(int n, complex in[], complex out[]);
void c_abs(int n, complex f[], float out[]);
void c_plus(complex a, complex b, complex *c);
void c_sub(complex a, complex b, complex *c);
void c_mul(complex a, complex b, complex *c);
void c_div(complex a, complex b, complex *c);
void Wn_i(int n, int i, complex *Wn, char flag);
void fft(int Samp_num, complex f[]);
void ifft(int Samp_num, complex f[]);
