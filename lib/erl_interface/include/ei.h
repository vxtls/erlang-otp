/*
 * %CopyrightBegin%
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright Ericsson AB 1998-2025. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * %CopyrightEnd%
 */
#ifndef EI_H
#define EI_H

#define EI_HAVE_TIMEOUT 1	/* Flag to user code that we have them */
#define USE_EI_UNDOCUMENTED     /* Want declarations for undocumented */

/************************************************************************/
/*          This file defines the complete interface to ei              */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*                   Include types needed below                         */
/* -------------------------------------------------------------------- */

#if defined(__WIN32__)
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
typedef LONG_PTR ssize_t; /* Sigh... */
#else
#include <sys/types.h>          /* ssize_t */
#endif

#include <stdio.h>		/* Need type FILE */
#include <errno.h>		/* Need EHOSTUNREACH, ENOMEM, ... */

#if !(defined(__WIN32__) || defined(_WIN32))
# include <netdb.h>
#endif

#ifdef __has_attribute
#if __has_attribute(deprecated)
#define EI_HAVE_DEPRECATED_ATTR__ 1
#else
#undef EI_HAVE_DEPRECATED_ATTR__
#endif
#endif

#ifdef EI_NO_DEPR_WARN
#undef EI_HAVE_DEPRECATED_ATTR__
#endif

#ifdef EI_HAVE_DEPRECATED_ATTR__
#define EI_DEPRECATED_ATTR_NAME deprecated
#define EI_DEPRECATED_ATTR __attribute__((EI_DEPRECATED_ATTR_NAME))
#else
#define EI_DEPRECATED_ATTR_NAME
#define EI_DEPRECATED_ATTR
#endif

/* -------------------------------------------------------------------- */
/*                      Defines part of API                             */
/* -------------------------------------------------------------------- */

/*
 * Some error codes might be missing, so here's a backstop definitions
 * of the ones we use with `erl_errno': 
 */

#ifndef EMSGSIZE		/* Message too long */
#define EMSGSIZE        EIO
#endif

#ifndef ETIMEDOUT		/* Connection timed out */
#define ETIMEDOUT       EIO
#endif

#ifndef EHOSTUNREACH		/* No route to host */
#define EHOSTUNREACH    EIO
#endif

/* FIXME just a few are documented, does it mean they can't be returned? */

#define ERL_ERROR -1           /* Error of some kind */
#define ERL_NO_DAEMON -2       /* No contact with EPMD */
#define ERL_NO_PORT -3         /* No port received from EPMD */   
#define ERL_CONNECT_FAIL -4    /* Connect to Erlang Node failed */
#define ERL_TIMEOUT -5         /* A timeout has expired */
#define ERL_NO_REMOTE -6       /* Cannot execute rsh */

#define ERL_TICK 0    
#define ERL_MSG 1    

#define ERL_NO_TIMEOUT -1

/* these are the control message types */
#define ERL_LINK           1
#define ERL_SEND           2
#define ERL_EXIT           3
#define ERL_UNLINK         4
#define ERL_NODE_LINK      5
#define ERL_REG_SEND       6
#define ERL_GROUP_LEADER   7
#define ERL_EXIT2          8
#define ERL_PASS_THROUGH  'p'

/* new ones for tracing, from Kenneth */
#define ERL_SEND_TT        12
#define ERL_EXIT_TT        13
#define ERL_REG_SEND_TT    16
#define ERL_EXIT2_TT       18
#define ERL_MONITOR_P      19
#define ERL_DEMONITOR_P    20
#define ERL_MONITOR_P_EXIT 21

/* For ei_xrpc_to */
#define EI_RPC_FETCH_STDOUT 1
/* -------------------------------------------------------------------- */
/*           Defines used for ei_get_type_internal() output             */
/* -------------------------------------------------------------------- */
/* 
 * these are the term type indicators used in
 * the external (distribution) format
 */

/* FIXME we don't want to export these..... */

#define ERL_SMALL_INTEGER_EXT 'a'
#define ERL_INTEGER_EXT       'b'
#define ERL_FLOAT_EXT         'c'
#define NEW_FLOAT_EXT         'F'
#define ERL_ATOM_EXT          'd'
#define ERL_SMALL_ATOM_EXT    's'
#define ERL_ATOM_UTF8_EXT     'v'
#define ERL_SMALL_ATOM_UTF8_EXT 'w'
#define ERL_REFERENCE_EXT     'e'
#define ERL_NEW_REFERENCE_EXT 'r'
#define ERL_NEWER_REFERENCE_EXT 'Z'
#define ERL_PORT_EXT          'f'
#define ERL_NEW_PORT_EXT      'Y'
#define ERL_PID_EXT           'g'
#define ERL_NEW_PID_EXT       'X'
#define ERL_SMALL_TUPLE_EXT   'h'
#define ERL_LARGE_TUPLE_EXT   'i'
#define ERL_NIL_EXT           'j'
#define ERL_STRING_EXT        'k'
#define ERL_LIST_EXT          'l'
#define ERL_BINARY_EXT        'm'
#define ERL_BIT_BINARY_EXT    'M'
#define ERL_SMALL_BIG_EXT     'n'
#define ERL_LARGE_BIG_EXT     'o'
#define ERL_NEW_FUN_EXT	      'p'
#define ERL_MAP_EXT           't'
#define ERL_FUN_EXT	      'u'
#define ERL_EXPORT_EXT        'q'
#define ERL_V4_PORT_EXT       'x'

 
#define ERL_NEW_CACHE         'N' /* c nodes don't know these two */
#define ERL_CACHED_ATOM       'C'


/* -------------------------------------------------------------------- */
/*                     Define the erl_errno macro                       */
/* -------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * GCC's attributes are too useful to not use. Other compilers
 * just lose opportunities to optimize and warn.
 */
#if !defined(__GNUC__) || __GNUC__ < 2
# define __attribute__(foo) /* nothing */
#endif

/*
 * Define the 'erl_errno' facility. Unfortunately this lives on in
 * the 'ei' interface as well.... :-(
 */

#if defined(_REENTRANT) || defined(__WIN32__)

/* 'erl_errno' as a function return value */
volatile int* __erl_errno_place(void) __attribute__ ((__const__));

#define erl_errno (*__erl_errno_place ())

#else /* !_REENTRANT && !__WIN32__ */

extern volatile int __erl_errno;

#define erl_errno __erl_errno

#endif /* !_REENTRANT && !__WIN32__ */


/* -------------------------------------------------------------------- */
/*                      Type definitions                                */
/* -------------------------------------------------------------------- */

#ifdef __WIN32__
#define EI_LONGLONG __int64
#define EI_ULONGLONG unsigned __int64
#else
#define EI_LONGLONG long long
#define EI_ULONGLONG unsigned long long
#endif
    
/*
 * To avoid confusion about the MAXHOSTNAMELEN when compiling the
 * library and when using the library we set a value that we use
 */

#define EI_MAX_COOKIE_SIZE 512
#define MAXATOMLEN (255 + 1)
#define MAXATOMLEN_UTF8 (255*4 + 1)
#define EI_MAXHOSTNAMELEN (MAXATOMLEN - 2)
#define EI_MAXALIVELEN (MAXATOMLEN - 2)
#define MAXNODELEN MAXATOMLEN

typedef enum { 
    ERLANG_ASCII = 1,
    ERLANG_LATIN1 = 2,
    ERLANG_UTF8 = 4
}erlang_char_encoding;

/* a pid */
typedef struct {
  char node[MAXATOMLEN_UTF8];
  unsigned int num;
  unsigned int serial;
  unsigned int creation;
} erlang_pid;

/* a port */
typedef struct {
  char node[MAXATOMLEN_UTF8];
  EI_ULONGLONG id;
  unsigned int creation;
} erlang_port;

/* a ref */
typedef struct {
  char node[MAXATOMLEN_UTF8];
  int len;
  unsigned int n[5];
  unsigned int creation;
} erlang_ref;

/* a trace token */
typedef struct {
  long serial;
  long prev;
  erlang_pid from;
  long label;
  long flags;
} erlang_trace;

/* a message */
typedef struct {
  long msgtype;
  erlang_pid from;
  erlang_pid to;
  char toname[MAXATOMLEN_UTF8];
  char cookie[MAXATOMLEN_UTF8];
  erlang_trace token;
} erlang_msg;

/* a fun */
typedef struct {
    long arity;
    char module[MAXATOMLEN_UTF8];
    enum { EI_FUN_CLOSURE, EI_FUN_EXPORT } type;
    union {
        struct {
            char md5[16];
            long index;
            long old_index;
            long uniq;
            long n_free_vars;
            erlang_pid pid;
            long free_var_len;
            char* free_vars;
        } closure;
        struct {
            char* func;
            int func_allocated;
        } exprt;
    } u;
} erlang_fun;

/* a big */
typedef struct {
    unsigned int arity;
    int is_neg;
    void *digits;
} erlang_big;

typedef struct {
    char ei_type;
    int arity;
    int size;
    union {
	long i_val;
	double d_val;
	char atom_name[MAXATOMLEN_UTF8];
	erlang_pid pid;
	erlang_port port;
	erlang_ref ref;
    } value;
} ei_term;

/* XXX */

typedef struct {
  char ipadr[4];             /* stored in network byte order */
  char nodename[MAXNODELEN+1];
} ErlConnect;

#define EI_SCLBK_INF_TMO (~((unsigned) 0))

#define EI_SCLBK_FLG_FULL_IMPL (1 << 0)

/*
 * HACK: AIX defines many socket functions like accept to be naccept, which
 * pollutes the global namespace. Set up an ugly ifdef for consumers of this
 * API here so they get a mangled name for AIX and the sane name elsewhere.
 */
#ifdef _AIX
#define EI_ACCEPT_NAME accept_ei
#else
#define EI_ACCEPT_NAME accept
#endif

typedef struct {
    int flags;

    int (*socket)(void **ctx, void *setup_ctx);
    int	(*close)(void *ctx);
    int (*listen)(void *ctx, void *addr, int *len, int backlog);
    int (*EI_ACCEPT_NAME)(void **ctx, void *addr, int *len, unsigned tmo);
    int (*connect)(void *ctx, void *addr, int len, unsigned tmo);
    int (*writev)(void *ctx, const void *iov, int iovcnt, ssize_t *len, unsigned tmo);
    int (*write)(void *ctx, const char *buf, ssize_t *len, unsigned tmo);
    int (*read)(void *ctx, char *buf, ssize_t *len, unsigned tmo);

    int (*handshake_packet_header_size)(void *ctx, int *sz);
    int (*connect_handshake_complete)(void *ctx);
    int (*accept_handshake_complete)(void *ctx);
    int (*get_fd)(void *ctx, int *fd);

    /* end of version 1 */
    
} ei_socket_callbacks;

typedef struct ei_cnode_s {
    char thishostname[EI_MAXHOSTNAMELEN+1];
    char thisnodename[MAXNODELEN+1];
    char thisalivename[EI_MAXALIVELEN+1];
/* Currently this_ipaddr isn't used */
/*    struct in_addr this_ipaddr; */
    char ei_connect_cookie[EI_MAX_COOKIE_SIZE+1];
    unsigned int creation;
    erlang_pid self;
    ei_socket_callbacks *cbs;
    void *setup_context;
    unsigned int pidsn;
} ei_cnode;

typedef struct in_addr *Erl_IpAddr; 


/* A dynamic version of ei XX */

typedef struct ei_x_buff_TAG {
    char* buff;
    int buffsz;
    int index;
} ei_x_buff;

/* -------------------------------------------------------------------- */
/*    Function definitions (listed in same order as documentation)      */
/* -------------------------------------------------------------------- */

/* Handle the connection */

int ei_connect_init(ei_cnode* ec, const char* this_node_name,
		    const char *cookie, unsigned int creation);
int ei_connect_xinit (ei_cnode* ec, const char *thishostname,
		      const char *thisalivename, const char *thisnodename,
		      Erl_IpAddr thisipaddr, const char *cookie,
		      unsigned int creation);

int ei_connect_init_ussi(ei_cnode* ec, const char* this_node_name,
                         const char *cookie, unsigned int creation,
                         ei_socket_callbacks *cbs, int cbs_sz,
                         void *setup_context);
int ei_connect_xinit_ussi(ei_cnode* ec, const char *thishostname,
                          const char *thisalivename, const char *thisnodename,
                          Erl_IpAddr thisipaddr, const char *cookie,
                          unsigned int creation, ei_socket_callbacks *cbs,
                          int cbs_sz, void *setup_context);

int ei_connect(ei_cnode* ec, char *nodename);
int ei_connect_tmo(ei_cnode* ec, char *nodename, unsigned ms);
int ei_connect_host_port(ei_cnode* ec, char *hostname, int port);
int ei_connect_host_port_tmo(ei_cnode* ec, char *hostname, int port, unsigned ms);
int ei_xconnect(ei_cnode* ec, Erl_IpAddr adr, char *alivename);
int ei_xconnect_tmo(ei_cnode* ec, Erl_IpAddr adr, char *alivename, unsigned ms);
int ei_xconnect_host_port(ei_cnode* ec, Erl_IpAddr adr, int port);
int ei_xconnect_host_port_tmo(ei_cnode* ec, Erl_IpAddr adr, int port, unsigned ms);

int ei_receive(int fd, unsigned char *bufp, int bufsize);
int ei_receive_tmo(int fd, unsigned char *bufp, int bufsize, unsigned ms);
int ei_receive_msg(int fd, erlang_msg* msg, ei_x_buff* x);
int ei_receive_msg_tmo(int fd, erlang_msg* msg, ei_x_buff* x, unsigned ms);
int ei_xreceive_msg(int fd, erlang_msg* msg, ei_x_buff* x);
int ei_xreceive_msg_tmo(int fd, erlang_msg* msg, ei_x_buff* x, unsigned ms);

int ei_send(int fd, erlang_pid* to, char* buf, int len);
int ei_send_tmo(int fd, erlang_pid* to, char* buf, int len, unsigned ms);
int ei_reg_send(ei_cnode* ec, int fd, char *server_name, char* buf, int len);
int ei_reg_send_tmo(ei_cnode* ec, int fd, char *server_name, char* buf, int len, unsigned ms);

int ei_rpc(ei_cnode* ec, int fd, char *mod, char *fun,
	   const char* inbuf, int inbuflen, ei_x_buff* x);
int ei_xrpc_to(ei_cnode* ec, int fd, char *mod, char *fun,
               const char* buf, int len, int flags);
int ei_rpc_to(ei_cnode* ec, int fd, char *mod, char *fun,
	      const char* buf, int len);
int ei_rpc_from(ei_cnode* ec, int fd, int timeout, erlang_msg* msg,
		ei_x_buff* x);

int ei_publish(ei_cnode* ec, int port);
int ei_publish_tmo(ei_cnode* ec, int port, unsigned ms);
int ei_listen(ei_cnode *ec, int *port, int backlog);
int ei_xlisten(ei_cnode *ec, Erl_IpAddr adr, int *port, int backlog);    
int ei_accept(ei_cnode* ec, int lfd, ErlConnect *conp);
int ei_accept_tmo(ei_cnode* ec, int lfd, ErlConnect *conp, unsigned ms);
int ei_unpublish(ei_cnode* ec);
int ei_unpublish_tmo(const char *alive, unsigned ms);

int ei_close_connection(int fd);

const char *ei_thisnodename(const ei_cnode* ec);
const char *ei_thishostname(const ei_cnode* ec);
const char *ei_thisalivename(const ei_cnode* ec);

erlang_pid *ei_self(ei_cnode* ec);
int ei_make_pid(ei_cnode *ec, erlang_pid *pid);
int ei_make_ref(ei_cnode *ec, erlang_ref *ref);

/* 
 * settings 
 */

void ei_set_compat_rel(unsigned rel);
void ei_set_tracelevel(int);
int ei_get_tracelevel(void);

/* 
 * We have erl_gethost*() so we include ei versions as well.
 */

struct hostent *ei_gethostbyname(const char *name);
struct hostent *ei_gethostbyaddr(const char *addr, int len, int type);
struct hostent *ei_gethostbyname_r(const char *name, 
				   struct hostent *hostp, 
				   char *buffer, 
				   int buflen, 
				   int *h_errnop);
struct hostent *ei_gethostbyaddr_r(const char *addr,
				   int length, 
				   int type, 
				   struct hostent *hostp,
				   char *buffer,  
				   int buflen, 
				   int *h_errnop);


/* Encode/decode functions */

int ei_encode_version(char *buf, int *index);
int ei_x_encode_version(ei_x_buff* x);
int ei_encode_long(char *buf, int *index, long p);
int ei_x_encode_long(ei_x_buff* x, long n);
int ei_encode_ulong(char *buf, int *index, unsigned long p);
int ei_x_encode_ulong(ei_x_buff* x, unsigned long n);
int ei_encode_double(char *buf, int *index, double p);
int ei_x_encode_double(ei_x_buff* x, double dbl);
int ei_encode_boolean(char *buf, int *index, int p);
int ei_x_encode_boolean(ei_x_buff* x, int p);
int ei_encode_char(char *buf, int *index, char p);
int ei_x_encode_char(ei_x_buff* x, char p);
int ei_encode_string(char *buf, int *index, const char *p);
int ei_encode_string_len(char *buf, int *index, const char *p, int len);
int ei_x_encode_string(ei_x_buff* x, const char* s);
int ei_x_encode_string_len(ei_x_buff* x, const char* s, int len);
int ei_encode_atom(char *buf, int *index, const char *p);
int ei_encode_atom_as(char *buf, int *index, const char *p,
		    erlang_char_encoding from, erlang_char_encoding to);
int ei_encode_atom_len(char *buf, int *index, const char *p, int len);
int ei_encode_atom_len_as(char *buf, int *index, const char *p, int len,
			erlang_char_encoding from, erlang_char_encoding to);
int ei_x_encode_atom(ei_x_buff* x, const char* s);
int ei_x_encode_atom_as(ei_x_buff* x, const char* s,
		      erlang_char_encoding from, erlang_char_encoding to);
int ei_x_encode_atom_len(ei_x_buff* x, const char* s, int len);
int ei_x_encode_atom_len_as(ei_x_buff* x, const char* s, int len,
			  erlang_char_encoding from, erlang_char_encoding to);
int ei_encode_binary(char *buf, int *index, const void *p, long len);
int ei_encode_bitstring(char *buf, int *index, const char *p, size_t bitoffs, size_t bits);
int ei_x_encode_binary(ei_x_buff* x, const void* s, int len);
int ei_x_encode_bitstring(ei_x_buff* x, const char* p, size_t bitoffs, size_t bits);
int ei_encode_pid(char *buf, int *index, const erlang_pid *p);
int ei_x_encode_pid(ei_x_buff* x, const erlang_pid* pid);
int ei_encode_fun(char* buf, int* index, const erlang_fun* p);
int ei_x_encode_fun(ei_x_buff* x, const erlang_fun* fun);
int ei_encode_port(char *buf, int *index, const erlang_port *p);
int ei_x_encode_port(ei_x_buff* x, const erlang_port *p);
int ei_encode_ref(char *buf, int *index, const erlang_ref *p);
int ei_x_encode_ref(ei_x_buff* x, const erlang_ref *p);
int ei_encode_trace(char *buf, int *index, const erlang_trace *p);
int ei_x_encode_trace(ei_x_buff* x, const erlang_trace *p);
int ei_encode_tuple_header(char *buf, int *index, int arity);
int ei_x_encode_tuple_header(ei_x_buff* x, long n);
int ei_encode_list_header(char *buf, int *index, int arity);
int ei_x_encode_list_header(ei_x_buff* x, long n);
#define ei_encode_empty_list(buf,i) ei_encode_list_header(buf,i,0)
int ei_x_encode_empty_list(ei_x_buff* x);
int ei_encode_map_header(char *buf, int *index, int arity);
int ei_x_encode_map_header(ei_x_buff* x, long n);

/* 
 * ei_get_type() returns the type and "size" of the item at
 * buf[index]. For strings and atoms, size is the number of characters
 * not including the terminating 0. For binaries, size is the number
 * of bytes. For lists and tuples, size is the arity of the
 * object. For other types, size is 0. In all cases, index is left
 * unchanged.
 */

int ei_get_type(const char *buf, const int *index, int *type, int *size);

/* Step through buffer, decoding the given type into the buffer
 * provided. On success, 0 is returned and index is updated to point
 * to the start of the next item in the buffer. If the type of item at
 * buf[index] is not the requested type, -1 is returned and index is
 * not updated. The buffer provided by the caller must be sufficiently
 * large to contain the decoded object.
 */
int ei_decode_version(const char *buf, int *index, int *version);
int ei_decode_long(const char *buf, int *index, long *p);
int ei_decode_ulong(const char *buf, int *index, unsigned long *p);
int ei_decode_double(const char *buf, int *index, double *p);
int ei_decode_boolean(const char *buf, int *index, int *p);
int ei_decode_char(const char *buf, int *index, char *p);
int ei_decode_string(const char *buf, int *index, char *p);
int ei_decode_atom(const char *buf, int *index, char *p);
int ei_decode_atom_as(const char *buf, int *index, char *p, int destlen, erlang_char_encoding want, erlang_char_encoding* was, erlang_char_encoding* result);
int ei_decode_binary(const char *buf, int *index, void *p, long *len);
int ei_decode_bitstring(const char *buf, int *index, const char** pp,
                        unsigned int* bitoffsp, size_t *nbitsp);

int ei_decode_fun(const char* buf, int* index, erlang_fun* p);
void free_fun(erlang_fun* f);
int ei_decode_pid(const char *buf, int *index, erlang_pid *p);
int ei_decode_port(const char *buf, int *index, erlang_port *p);
int ei_decode_ref(const char *buf, int *index, erlang_ref *p);
int ei_decode_trace(const char *buf, int *index, erlang_trace *p);
int ei_decode_tuple_header(const char *buf, int *index, int *arity);
int ei_decode_list_header(const char *buf, int *index, int *arity);
int ei_decode_map_header(const char *buf, int *index, int *arity);
int ei_decode_iodata(const char *buf, int* index, int *szp, char *out_buf);

/* 
 * ei_decode_ei_term() returns 1 if term is decoded, 0 if term is OK,
 * but not decoded here and -1 if something is wrong.  ONLY changes
 * index if term is decoded (return value 1)!
 */

int ei_decode_ei_term(const char* buf, int* index, ei_term* term);


/*
 * ei_print_term to print out a binary coded term
 */

int ei_print_term(FILE *fp, const char* buf, int* index);
int ei_s_print_term(char** s, const char* buf, int* index);

/*
 * format to build binary format terms a bit like printf
 */

int ei_x_format(ei_x_buff* x, const char* fmt, ...);
int ei_x_format_wo_ver(ei_x_buff* x, const char *fmt, ...);

int ei_x_new(ei_x_buff* x);
int ei_x_new_with_version(ei_x_buff* x);
int ei_x_free(ei_x_buff* x);
int ei_x_append(ei_x_buff* x, const ei_x_buff* x2);
int ei_x_append_buf(ei_x_buff* x, const char* buf, int len);
int ei_skip_term(const char* buf, int* index);

int ei_cmp_refs(erlang_ref *a, erlang_ref *b);
int ei_cmp_pids(erlang_pid *a, erlang_pid *b);
int ei_cmp_ports(erlang_port *a, erlang_port *b);

/* -------------------------------------------------------------------- */
/* Initialize erl_interface                                             */
/* -------------------------------------------------------------------- */

int ei_init(void);

/* -------------------------------------------------------------------- */
/*            The ei_global functions */
/* -------------------------------------------------------------------- */
char **ei_global_names(ei_cnode *ec, int fd, int *count);
int ei_global_whereis(ei_cnode *ec, int fd, const char *name, erlang_pid* pid, char *node);
int ei_global_register(int fd, const char *name, erlang_pid *self);
int ei_global_unregister(ei_cnode *ec, int fd, const char *name);

/* -------------------------------------------------------------------- */
/*            Encoding/decoding bugnums to GNU MP format                */
/* -------------------------------------------------------------------- */

/* If the user included <gmp.h> we supply some more functions */

#if defined(__GNU_MP_VERSION) \
	&& __GNU_MP_VERSION == 4 && __GNU_MP_VERSION_MINOR >= 1 

int ei_decode_bignum(const char *buf, int *index, mpz_t obj);
int ei_encode_bignum(char *buf, int *index, mpz_t obj);
int ei_x_encode_bignum(ei_x_buff *x, mpz_t obj);

#endif /* __GNU_MP_VERSION */

/* -------------------------------------------------------------------- */
/*            Function definitions not documented FIXME                 */
/* -------------------------------------------------------------------- */

/* FIXME replace this primitive type size code */

int ei_decode_longlong(const char *buf, int *index, EI_LONGLONG *p);
int ei_decode_ulonglong(const char *buf, int *index, EI_ULONGLONG *p);
int ei_encode_longlong(char *buf, int *index, EI_LONGLONG p);
int ei_encode_ulonglong(char *buf, int *index, EI_ULONGLONG p);
int ei_x_encode_longlong(ei_x_buff* x, EI_LONGLONG n);
int ei_x_encode_ulonglong(ei_x_buff* x, EI_ULONGLONG n);

#ifdef USE_EI_UNDOCUMENTED

/* 
 * Decode a list of integers into an integer array (i.e. even if it is
 * encoded as a string). count gets number of items in array.
 * See "decode_intlist.c".
 */

int ei_decode_intlist(const char *buf, int *index, long *a, int *count);

/* 
 * FIXME: used in IC, document?
 * bufp = address of pointer to dynamically allocated buffer - may be reallocated by
 * this function if it is too small for the message
 * bufsz = in/out value: in=user buffer size, out=new buffer size
 * msglen = nr bytes in received message
 */
int ei_receive_encoded(int fd, char **bufp, int *bufsz, erlang_msg *to, 
		       int *msglen);
int ei_receive_encoded_tmo(int fd, char **bufp, int *bufsz, erlang_msg *to, 
			   int *msglen, unsigned ms);
int ei_send_encoded(int fd, const erlang_pid *to, char *msg, int msglen);
int ei_send_encoded_tmo(int fd, const erlang_pid *to, char *msg, int msglen,
			unsigned ms);
int ei_send_reg_encoded(int fd, const erlang_pid *from, const char *to,
			char *msg, int msglen);
int ei_send_reg_encoded_tmo(int fd, const erlang_pid *from, const char *to,
			    char *msg, int msglen, unsigned ms);

/*
 * Bacward compatibility with old undocumented but used interface...
 * FIXME use wrapper function instead?!
 */
#define ei_send_encoded_timeout(Fd,To,Msg,MsgLen,Ms) \
    ei_send_encoded_tmo((Fd),(To),(Msg),(MsgLen),(Ms))
#define ei_send_reg_encoded_timeout(Fd,From,To,Msg,MsgLen,Ms) \
    ei_send_reg_encoded_tmo((Fd),(From),(To),(Msg),(MsgLen),(Ms))


/* FIXME: is this really the best way to handle bignums? */
int ei_encode_big(char *buf, int *index, erlang_big* big);
int ei_x_encode_big(ei_x_buff* x, erlang_big* big);
int ei_decode_big(const char *buf, int *index, erlang_big* p);
int ei_big_comp(erlang_big *x, erlang_big *y);
int ei_big_to_double(erlang_big *b, double *resp);
int ei_small_to_big(int s, erlang_big *b);
erlang_big *ei_alloc_big(unsigned int arity);
void ei_free_big(erlang_big *b);

#endif /* USE_EI_UNDOCUMENTED */

#ifdef __cplusplus
}
#endif

#endif /* EI_H */
