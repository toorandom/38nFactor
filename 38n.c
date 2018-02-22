#include <openssl/bn.h>
#include <pthread.h>
#include <stdio.h>
#define MAX_BITS 1024*1024

BIGNUM *one,*five,*tone;
unsigned char *bin;



void
print_bin (unsigned char *bin, int nbits)
{

  int i = 0, j, b = 0;
  while (nbits > 0)
    {
      for (j = 0; j < 8; j++)
	{
	  printf ("%d", (bin[i] >> (7 - j)) & 1);
	  nbits--;
	  if (nbits == 0)
	    {
	      break;
	      b = 1;
	    }
	}
      if (b)
	break;
      i++;
    }

}



/* fix the positions of the bits "11" in the first and second bytes to build 3*8^n */
void
fix_bits (unsigned char *bits, unsigned int n)
{
  int v = (3 * n) % 8;
  bits[0] = 3;
  bits[1] = 0;
  if (v != 7)
    bits[0] = bits[0] << v;
  else
    {
      bits[0] = 1;
      bits[1] = 0x80;
    }



}


int
provable_prime (BIGNUM * p)
{

  BIGNUM *p_1 = BN_new ();
  BIGNUM *t = BN_new ();
  BN_CTX *ctx;

  ctx = BN_CTX_new ();
  BN_CTX_init (ctx);

  BN_sub (p_1, p, one);
  BN_CTX_start(ctx);
  BN_mod_exp (t, five, p_1, p, ctx);
  BN_CTX_end(ctx);

  if (BN_is_one (t))
    {
      BN_clear (t);
      BN_CTX_start(ctx);
      BN_mod_exp (t, tone, p_1, p, ctx);
      BN_CTX_end(ctx);
      BN_CTX_free (ctx);
      BN_free (p_1);
      if (BN_is_one (t))
	{
	  BN_free (t);
	  return 1;
	}
      else
	{
	  printf ("5 was a Fermat liar\n");
	  BN_free (t);
	  return 0;
	}
    }
  BN_free (t);
  BN_free (p_1);
  BN_CTX_free (ctx);
  return 0;

}

int check_prime_3x8n_1(unsigned int n) {
 BIGNUM *p,*x,*f;
 int ret;
 p = BN_new();
 x = BN_new();
 f = BN_new();
 BN_clear(f);
 BN_clear(x);
 BN_clear(p);

 fix_bits(bin,n);
 BN_bin2bn (bin, ((3 * n) + 1 + 8 - ((3 * n + 1) % 8)) / 8, x);
 BN_sub (f, x, one);
 ret = provable_prime(f);
 BN_free(f);
 BN_free(x);
 BN_free(p);
 return ret;
}


/*
int
main (int ac, char **argv)
{
  int lbound,rbound,n;

  bin = calloc (MAX_BITS, sizeof (char));
  x = BN_new ();
  one = BN_new ();
  f = BN_new ();


  BN_dec2bn (&one, "1");


lbound = atoi(argv[1]);
rbound = atoi(argv[2]);

  for (n = lbound; n < rbound; n++)
    {
		if(check_prime_3x8n_1(n)) 
			printf("For %d is prime\n",n);

    }

  return 0;
}
*/
