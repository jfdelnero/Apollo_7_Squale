typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

/* decode.c */

static unsigned  decode(unsigned count, uch buffer[]);
static void decode_start();

/* huf.c */
static void huf_decode_start();
static unsigned decode_c();
static unsigned decode_p();
static void read_pt_len(int nn, int nbit, int i_special);
static void read_c_len();

/* io.c */
static void fillbuf(int n);
static unsigned getbits(int n);
static void init_getbits();

/* maketbl.c */

static void make_table(int nchar, uch bitlen[], int tablebits, ush table[]);

int unlzh(unsigned char * packed_buf,int insize, unsigned char * unpacked_buf,int outsize);
