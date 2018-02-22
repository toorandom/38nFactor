/*
 * Programa que muestra como usar procesadores distintos en threads
 * aqui se detecta el numero de procesadores que tienes, en mi caso es 4. 
 * este programa crea threads que se autoasignan un procesador, los threads son creados
 * con un argumento el cual contiene una estructura que tendra datos a procesar distintos a cada thread
 * en este ejemplo procesara el promedio de los datos que se le mandan, a cada thread se le manda AAA.. , BBB.., CCC.., DDD... etc..
 * o sea que cada uno en promedio debera tener 0x41,0x42,0x43,0x44, etc.. hace un loop infinito para que observes el uso de tus procesadores
 *
 * compila:
 *
 * gcc -Wall -lpth estecodigo.c -o out
 *
 * ejecutas:
 *
 * ./out
 *
 * toorandom@gmail.com
 *
 * Eduardo Ruiz Duarte (beck)
 *
 */
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <openssl/bn.h>


extern unsigned char *bin;
extern BIGNUM *x,*f,*one,*five,*tone;


/* CPUs en tu maquina */
int num_cpus;			/* via sysconf() con _SC_NPROCESSORS_CONF */
/* Es algo asi como el buf size de lo que se manda a cada thread para procesar */
#define TAMANO_DATOS 256

#define MAX_BITS 1024*1024


/* Esto se le pasara a cada thread, el procesador que va a utilizar, un apuntador a algo para procesar y 
 * el tamanio de estos datos a procesar 
 */
typedef struct contexto
{
  long cpuid;
  int sb,eb;
  int datos_len;
  void *datos;

} contexto_t;

/* funcion que hara un proceso paralelo obtendra datos para el thread de ctx el cual tendra datos a procesar, 
 * cpu que se le asignara y tamanio de datos 
 */
void *
proceso_paralelo (void *ctx)
{
  long tid;			/* thread id */
  int err;
  contexto_t *info;		/* estructura para parsear ctx */
  cpu_set_t cpuset;
  pthread_t thread;
  info = ctx;
  tid = info->cpuid;
  thread = pthread_self ();
  CPU_ZERO (&cpuset);
  CPU_SET (tid, (cpu_set_t *) & cpuset);
  if (err=pthread_setaffinity_np (thread, sizeof (cpu_set_t), &cpuset) != 0) {
    printf("ERROR affinity = %d at thread = %ld\n",err,tid);
    perror ("pthread_setaffinity_np");
  }



   int n;
   for(n=info->sb+tid;n<info->eb; n += num_cpus) 
{
        if(n%4 != 3) {
	printf("Checking at processor id=%ld => power n=%d\n",tid,n);
	if(check_prime_3x8n_1(n)) {
		printf("[id=%ld]: Prime for n=%d\n",tid,n);
	}
      }
	//else
	//printf("[id=%d]: Skipping %d\n",tid,n);
}




  pthread_exit (NULL);
}

int
main (int argc, char *argv[])
{
  num_cpus = sysconf (_SC_NPROCESSORS_CONF);

  pthread_t *threads;	/*  threads que haremos */
  long t;			/* id de thread a crear */
   int err;
  int sb,eb;
  threads = calloc(num_cpus,sizeof(pthread_t));

  contexto_t *ctx;

  ctx = calloc(num_cpus,sizeof(contexto_t));
  memset (ctx, 0, sizeof (contexto_t)*num_cpus);

  bin = calloc (MAX_BITS, sizeof (char));
  one = BN_new ();
  tone = BN_new ();
  five = BN_new();
  BN_dec2bn (&one, "1");
  BN_dec2bn (&tone, "31");
  BN_dec2bn (&five, "2");

  if (argc < 3) { 
	printf("Need to give me a start n and a final n to check 3*8^n -1 to be prime\n");
	exit(-1);
  }
  sb = atoi(argv[1]);
  eb = atoi(argv[2]);


  printf ("El numero de procesadores que hay disponibles es %d\n", num_cpus);
  for (t = 0; t < num_cpus; t++)
    {
      printf ("Creando thread %ld\n", t);
      ctx[t].cpuid = t;
      ctx[t].sb = sb;
      ctx[t].eb = eb;
      if ((err=pthread_create
	  (&threads[t], NULL, proceso_paralelo, (void *) &ctx[t])) != 0)
	{
	  printf("ERROR=%d\n",err);
	  perror ("pthread_create");
	  exit (EXIT_FAILURE);
	}
    }

  pthread_exit (NULL);
}
