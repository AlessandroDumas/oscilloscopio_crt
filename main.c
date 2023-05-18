

#include "main.h"
#include "xiaolin_wu_line.h"

#include <stdio.h>

#define PI 3.14159265358979323846
//double PI=4*atan(1.0);

#define XSIZE 128*5
#define YSIZE 128*4

// immagine lissajous
uint16_t image[YSIZE][XSIZE];


typedef struct {
	int16_t x;
	int16_t y;
} coordinata;


struct draw_setup {
	float lum;
} draw_setup;


struct image_setup {
	uint16_t xsize;
	uint16_t ysize;
} image_setup;


/*
int cicli()
{
//scrive su stdout una sequenza di campioni S16 bit stereo (interlacciati)
//il segnale è il campionamento di 2 sinusoidi isofrequenziali
//la prima ha armoniche
//la seconda no
	unsigned int i;
	unsigned int t;
	int16_t xy[2];
	float phase;
	
	for(t=0; t<((5<<16)); t++)
	{
		phase=2*PI*t/65536*100.0;

		xy[0]=32768*pow(sin(phase),3);
		xy[1]=32768*sin(2*phase);
		
//		image[512+xy[0]>>6][512+xy[1]>>6]++;
		
		fwrite(xy, 4, 1, stdout);

	//	putw(x, stdout);
	//	putw(y, stdout);
	}
	
	return 0;
	
}
*/

//aggiunge un punto nell'immagine
//	centrato: (0,0) = centro immagine
int punto(int16_t x, int16_t y, int16_t val)
{
//	int16_t val=1;
	
	int16_t xx=image_setup.xsize/2+x;
	int16_t yy=image_setup.ysize/2+y;
	
	
	if(xx >= image_setup.xsize || xx < 0)
	{
//		fprintf(stderr, "x underflow %d\n",x);
		return -1;
	}
	if(yy >= image_setup.ysize || yy < 0 )
	{
//		fprintf(stderr, "y underflow %d\n",x);
		return -1;
	}
	
	
//	x=max(x,-image_setup.ysize/2);
//	x=min(x,image_setup.ysize/2);

	//punto singolo	
//	image[yy][xx]+=val*draw_setup.lum;
	image[yy][xx]=fmin(image[yy][xx]+val*draw_setup.lum,65535);
	
	return 0;
	
}


/*
coordinata leggibuffer(); 
{
	fread(stdin,)
}
*/


//linea
void linea(coordinata a, coordinata b)
{
	drawLine(a.x,a.y,b.x,b.y);
}


// disegna una griglia come un oscilloscopio
void griglia()
{
	//	|-|-|-|-|-|-|-|-|-|-|
	//	10x8
	//	11x9 linee

	uint8_t n,m;
	coordinata a,b,offset;
	
	offset.x=image_setup.xsize/2;
	offset.y=image_setup.ysize/2;

	// verticali
	for(n=0;n<11;n++)
	{
		a.x=n/10.0*image_setup.xsize -offset.x;
		a.y=0 -offset.y;
		b.x=a.x;
		b.y=image_setup.ysize -offset.y;
		
		linea(a,b);
	}

	// orizzontali
	for(m=0;m<9;m++)
	{
		a.x=0 -offset.x;
		a.y=m/8.0*image_setup.ysize -offset.y;
		b.x=image_setup.xsize -offset.x;
		b.y=a.y;

		linea(a,b);
	}
	
	

	/*
	 * prova
	 *
	a.x=-200;
	a.y=100;
	b.x=-150;
	b.y=500;

	linea(a,b);
	*/
}


void sinusoide()
{
	
	int16_t x,y;
	float f=100;	//frequenza sinusoide
	float Tx=20e-3;	//periodo scansione
	float t;	//tempo
	
	while(t<Tx)
        {
		x=image_setup.xsize*t/Tx-image_setup.xsize/2;
		y=image_setup.ysize/2*sin(2*PI*f*t);
		
		punto(x,y,1);
		t+=Tx/(1<<16);
	}
}


void lissajous()
{

	int16_t x,y;
	int t;
	float phase;
        for(t=0; t<(1<<17); t++)
        {
		phase=2*PI*t/65536*100.0;

		x=(image_setup.xsize/4)*pow(sin(phase*7/16),3);
		y=(image_setup.ysize/4)*sin(2*phase);
		
		punto(x,y,1);
	}


}

//cancella immagine
void clear_img()
{
	int r; //riga
	int c; //col

	for(r=0;r<image_setup.ysize;r++)
		for(c=0;c<image_setup.xsize;c++)
			image[r][c]=0;

}

void gamma_img()
{
//applica gamma all'intero frame

	int r; //riga
	int c; //col

	//gamma
	for(r=0;r<image_setup.ysize;r++)
		for(c=0;c<image_setup.xsize;c++)
			image[r][c]=sqrt(image[r][c])*(1<<8);

}

//scrive immagine (raw) su stdout
void scrivi_img()
{
	int r; //riga

	//write
	for(r=0;r<image_setup.ysize;r++)
	{
//		fprintf(stderr, "riga %d\n",y);
		fwrite(image[r],2,image_setup.xsize,stdout);
	}

}

void leggi_stdin()
{
//leggi da stdin	
	
	coordinata p,a,b;

	float scala=image_setup.ysize/65536.0;

	while(fread(&p,4,1,stdin)!=0)
	{

		//punti
//		punto(p.x*scala,p.y*scala,1);
		
		//linee
		a.x=p.x*scala;
		a.y=p.y*scala;
		linea(a,b);
		b.x=a.x;
		b.y=a.y;

	}

	
}

void video()
{

	int punti_per_frame=2048;
	coordinata p,a,b;
	float dist;
	int f=0;
	int i;
	int r;

	float scala=image_setup.ysize/65536.0;

	while(!feof(stdin))
	{

		clear_img();

		draw_setup.lum=1.0*(1<<9);
		griglia();

		for(i=0;i<punti_per_frame;i++)
		{
			
		
			fread(&p,4,1,stdin);
			//punti
	//		punto(p.x*scala,p.y*scala,1);
			
			//linee
			a.x=p.x*scala;
			a.y=p.y*scala;

			dist=sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
//			draw_setup.lum=512.0/(dist);
			draw_setup.lum=512.0/(1+dist);
	
			linea(a,b);
			
			b.x=a.x;
			b.y=a.y;

		}
		
		//debug numero frame
//		fprintf(stderr, "frame %d\n",f);
		gamma_img();
		scrivi_img();

		f++;
	}
}

int main()
{

	image_setup.xsize=XSIZE;
	image_setup.ysize=YSIZE;
	draw_setup.lum=1;

	fprintf(stderr, "dimensione quadro: %d x %d\n",image_setup.xsize,image_setup.ysize);

//	griglia();
	
//	leggi_stdin();

//	lissajous();

//	sinusoide();	
	
//	scrivi_img();

	video();
	
	return 0;
	
}


