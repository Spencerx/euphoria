/* Euphoria
   C include file for Euphoria programs 
   that have been translated to C */

#undef _segment
#undef _self
#undef _dos_ds

/***************************************************************************
*
*
 32 bit number range:
  0X8      0XA      0XC      0XE      0X0      0X2      0X4      0X6      0X8
-4*2^29  -3*2^29  -2*2^29-1  -2^29   0*2^29   1*2^29   2*2^29   3*2^29 4*2^29 
   *--------*--------*--------*--------*--------*--------*--------*--------o
                     o NOVALUE = -2*2^29-1
		     o<-----------ATOM_INT---------[-2*2^29..4*2^29)------>o
	    |<----------------ATOM_DBL-------[-3*2^29..4*2^29)------------>o
-->|        |<-- IS_SEQUENCE [-4*2^29..-3*2^29)
-->|                 o<--- IS_DBL_OR_SEQUENCE [-4*2^29..-2*2^29-1)
*
****************************************************************************/

#define NOVALUE      ((long)0xbfffffffL)
#define IS_ATOM_INT(ob)       (((long)(ob)) > NOVALUE)
#define IS_ATOM_DBL(ob)         (((object)(ob)) >= (long)0xA0000000)
#define IS_ATOM(ob)             (((long)(ob)) >= (long)0xA0000000)
#define IS_SEQUENCE(ob)         (((long)(ob))  < (long)0xA0000000)
#define IS_DBL_OR_SEQUENCE(ob)  (((long)(ob)) < NOVALUE)
#define HIGH_BITS    ((long)0xC0000000L)

#undef MININT
#undef MAXINT
#define MININT     (long)0xC0000000
#define MAXINT     (long)0x3FFFFFFF
#define MAXINT_VAL MAXINT
#define MAXINT_DBL ((double)MAXINT_VAL)
#define INT15      (long)0x00003FFFL

#define LOW_MEMORY_MAX ((unsigned)0x0010FFEF)

typedef int object;
typedef int *object_ptr;

struct cleanup;
typedef struct cleanup *cleanup_ptr;
typedef void(*cleanup_func)(object);

struct cleanup {
	long type;
	union func_union{
		long rid;
		cleanup_func builtin;
	} func;
	cleanup_ptr next;
};

enum CLEANUP_TYPES {
	CLEAN_UDT,
	CLEAN_UDT_RT,
	CLEAN_PCRE,
	CLEAN_FILE
};

struct s1 {
	object_ptr base;
	long length;
	long ref;
	long postfill;
	cleanup_ptr cleanup;
};

struct d {
	double dbl;
	long ref;
	cleanup_ptr cleanup; 
};

struct routine_list {
	char *name;
	int (*addr)();
	int seq_num;
	int file_num;
	short int num_args;
	short int convention;
	char scope;
	cleanup_ptr cleanup;
};

struct ns_list {
	char *name;
	int ns_num;
	int seq_num;
	int file_num;
};


#define TASK_HANDLE int

struct interpreted_task{
	int *pc;         // program counter for this task
	object_ptr expr_stack; // call stack for this task
	object_ptr expr_max;   // current top limit of stack
	object_ptr expr_limit; // don't start a new routine above this
	object_ptr expr_top;   // stack pointer
	int stack_size;        // current size of stack
};

struct translated_task{
	TASK_HANDLE task;
	
};

// Task Control Block - sync with euphoria\include\euphoria.h
struct tcb {
	int rid;         // routine id
	double tid;      // external task id
	int type;        // type of task: T_REAL_TIME or T_TIME_SHARED
	int status;      // status: ST_ACTIVE, ST_SUSPENDED, ST_DEAD
	double start;    // start time of current run
	double min_inc;  // time increment for min
	double max_inc;  // time increment for max 
	double min_time; // minimum activation time
					 // or number of executions remaining before sharing
	double max_time; // maximum activation time (determines task order)
	int runs_left;   // number of executions left in this burst
	int runs_max;    // maximum number of executions in one burst
	int next;        // index of next task of the same kind
	object args;     // args to call task procedure with at startup
	
	int mode;  // TRANSLATED_TASK or INTERPRETED_TASK
	union task_impl {
		struct interpreted_task interpreted;
		struct translated_task translated;
	} impl;
	
};

typedef struct d  *d_ptr;
typedef struct s1 *s1_ptr;

#define MAKE_DBL(x) ( (int) (((unsigned)(x) >> 3) + (long)0xA0000000) )
#define DBL_PTR(ob) ( (d_ptr)  (((unsigned)(ob)) << 3) )
#define MAKE_SEQ(x) ( (int) (((unsigned)(x) >> 3) + (long)0x80000000) )
#define SEQ_PTR(ob) ( (s1_ptr) (((unsigned)(ob)) << 3) ) 

#define RefDS(a) ++(DBL_PTR(a)->ref)    
#define RefDSn(a,n) (DBL_PTR(a)->ref += n)    
#define Ref(a) if (IS_DBL_OR_SEQUENCE(a)) { RefDS(a); }
#define Refn(a,n) if (IS_DBL_OR_SEQUENCE(a)) { RefDSn(a,n); }

#define DeRefDS(a) if (--(DBL_PTR(a)->ref) == 0 ) { de_reference((s1_ptr)(a)); }
#define DeRefDSi(a) if (--(DBL_PTR(a)->ref) == 0 ) { de_reference_i((s1_ptr)(a)); }

#define DeRef(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDS(a); }
#define DeRefi(a) if (IS_DBL_OR_SEQUENCE(a)) { DeRefDSi(a); }

#define UNIQUE(seq) (((s1_ptr)(seq))->ref == 1)

#define EF_READ 1

#define LESS       1
#define GREATEREQ  2
#define EQUALS     3
#define NOTEQ      4
#define LESSEQ     5
#define GREATER    6
#define NOT        7
#define AND        8
#define OR         9
#define MINUS     10
#define PLUS      11
#define UMINUS    12
#define MULTIPLY  13
#define DIVIDE    14
#define OR_BITS   24  
#define XOR_BITS  26
#define SQRT      41
#define NOT_BITS  51
#define AND_BITS  56
#define RAND      62
#define FLOOR_DIV 63
#define REMAINDER 71
#define POWER     72
#define ARCTAN    73
#define LOG       74
#define SIN       80
#define COS       81
#define TAN       82
#define FLOOR     83
#define XOR      154

int wingetch();
int call_c();
int Command_Line();
void show_console();
int Dpower();
int Dand_bits();
int Dand();
int Dremainder();
void Cleanup();
void init_literal();
char **make_arg_cv();
int NewDouble(double);
void DeRef5(int, int, int, int, int);
void de_reference(s1_ptr);
void de_reference_i(s1_ptr);
double current_time(void);
double floor(double);
double fabs(double);
int binary_op_a(int, int, int);
int binary_op(int, int, int);
void *which_file(int, int);
int unary_op(int, int);
int NewS1(int);
int compare(int, int);
unsigned long get_pos_int(char *, int);
int memory_set(int, int, int);
int memory_copy(int, int, int);
int EOpen(int, int,int);
void EClose(int);
int EPrintf(int, int, int);
void EPuts(int, int);
void Concat(int *, int, s1_ptr);
void Concat_N(int *, int **, int);
void Append(int *, int, int);
void Prepend(int *, int, int);
int EGetenv(s1_ptr);
void RHS_Slice(s1_ptr, int, int);
int find(int, int);
int e_match(int, int);
void ctrace(char *);
int e_floor(int);
int DoubleToInt(int);
int machine(int, int);
int Repeat(int, int);
void RepeatElem(int, int, int);
int SequenceCopy(s1_ptr);
int power(int, int);
int EGets(int);
int Pixel(int, int);
int Get_Pixel(int);
void shift_args(int, char**);
int NewString(char *);
char *malloc(int);
void eu_startup();
void exit(int);
int CRoutineId(int, int, int);
int e_sqrt(int);
int e_arctan(int);
void AssignSlice(int, int, int);
void StdPrint(int, int, int);
void ClearScreen();
void Position(int, int);
int CommandLine(void);
void system_call(int, int);
void RTFatal(char *);
int e_match_from(int,int,int);
int find_from(int,int,int);
void Replace(int);
char *TransAlloc(unsigned long);