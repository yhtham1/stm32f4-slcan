//ＵＴＦ８
#ifndef _USART_H
#define _USART_H
#include "misc.h"

// intは【32Bit】です。
typedef unsigned int uint;


void init_usart1( uint baudrate );
void    putcSIO1( char c );
void    putsSIO1( char *s );
int           l1printf( char *format, ... );
void intr_USART1_rx( void );
void intr_USART1_tx( void );
int     getcSIO1( void );

void init_usart2( uint baudrate );
void    putcSIO2( char c );
void    putsSIO2( char *s );
int           l2printf( char *format, ... );
void intr_USART2_rx( void );
void intr_USART2_tx( void );
int     getcSIO2( void );
char getcSIO2b(void);


void init_usart3( uint baudrate );
void    putcSIO3( char c );
void    putsSIO3( char *s );
int           l3printf( char *format, ... );
void intr_USART3_rx( void );
void intr_USART3_tx( void );
int     getcSIO3( void );

void init_usart4( uint baudrate );
void    putcSIO4( char c );
void    putsSIO4( char *s );
int           l4printf( char *format, ... );
void intr_USART4_rx( void );
void intr_USART4_tx( void );
int     getcSIO4( void );

void  init_uart5( uint baudrate );
void    putcSIO5( char c );
void    putsSIO5( char *s );
int           l5printf( char *format, ... );
void intr_USART5_rx( void );
void intr_USART5_tx( void );
int     getcSIO5( void );


void init_usart6( uint baudrate );
void    putcSIO6( char c );
void    putsSIO6( char *s );
int           l6printf( char *format, ... );
void intr_USART6_rx( void );
void intr_USART6_tx( void );
int     getcSIO6( void );

void init_usart7( uint baudrate );
void    putcSIO7( char c );
void    putsSIO7( char *s );
int           l7printf( char *format, ... );
void intr_USART7_rx( void );
void intr_USART7_tx( void );
int     getcSIO7( void );

void init_usart8( uint baudrate );
void    putcSIO8( char c );
void    putsSIO8( char *s );
int           l8printf( char *format, ... );
void intr_USART8_rx( void );
void intr_USART8_tx( void );
int     getcSIO8( void );

#endif
