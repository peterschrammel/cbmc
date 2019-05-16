// tag-#anon#ST[*{SYM#tag-pcap#}$SYM#tag-pcap#$'pcap_desc'|SYM#tag-bpf_program#'fp'|U32'netp'|U32'netmask'|*{S8}$S8$'pcap_err_buf'|*{S8}$S8$'pcap_data_buf'|SYM#tag-#anon#ST[*{SYM#tag-pqueue_elem_t#}$SYM#tag-pqueue_elem_t#$'head'|*{SYM#tag-pqueue_elem_t#}$SYM#tag-pqueue_elem_t#$'tail'|S32'elems'|U32'$pad0']#'pkt_q']
// file ptunnel.h line 378
struct anonymous$5;

// tag-#anon#ST[*{SYM#tag-pqueue_elem_t#}$SYM#tag-pqueue_elem_t#$'head'|*{SYM#tag-pqueue_elem_t#}$SYM#tag-pqueue_elem_t#$'tail'|S32'elems'|U32'$pad0']
// file ptunnel.h line 370
struct anonymous$2;

// tag-#anon#ST[ARR16{S64}$S64$'__fds_bits']
// file /usr/include/x86_64-linux-gnu/sys/select.h line 64
struct anonymous;

// tag-#anon#ST[S32'pkt_len'|U32'$pad0'|F64'last_resend'|S32'resend_count'|U16'seq_no'|U16'icmp_id'|*{SYM#tag-#anon#ST[U8'type'|U8'code'|U16'checksum'|U16'identifier'|U16'seq'|ARR0{S8}$S8$'data']#}$SYM#tag-#anon#ST[U8'type'|U8'code'|U16'checksum'|U16'identifier'|U16'seq'|ARR0{S8}$S8$'data']#$'pkt']
// file ptunnel.h line 289
struct anonymous$8;

// tag-#anon#ST[S32'seq_no'|S32'length'|S32'remaining'|ARR0{S8}$S8$'data']
// file ptunnel.h line 275
struct anonymous$7;

// tag-#anon#ST[U32'magic'|U32'dst_ip'|U32'dst_port'|U32'state'|U32'ack'|U32'data_len'|U16'seq_no'|U16'id_no'|ARR0{S8}$S8$'data']
// file ptunnel.h line 200
struct anonymous$1;

// tag-#anon#ST[U8'type'|U8'code'|U16'checksum'|U16'identifier'|U16'seq'|ARR0{S8}$S8$'data']
// file ptunnel.h line 241
struct anonymous$6;

// tag-#anon#ST[U8'vers_ihl'|U8'tos'|U16'pkt_len'|U16'id'|U16'flags_frag_offset'|U8'ttl'|U8'proto'|U16'checksum'|U32'src_ip'|U32'dst_ip'|ARR0{S8}$S8$'data']
// file ptunnel.h line 217
struct anonymous$3;

// tag-#anon#UN[ARR4{S8}$S8$'__size'|S32'__align']
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 130
union anonymous$4;

// tag-#anon#UN[SYM#tag-__pthread_mutex_s#'__data'|ARR40{S8}$S8$'__size'|S64'__align']
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 90
union anonymous$0;

// tag-_IO_FILE
// file /usr/include/stdio.h line 44
struct _IO_FILE;

// tag-_IO_marker
// file /usr/include/libio.h line 160
struct _IO_marker;

// tag-__pthread_internal_list
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 75
struct __pthread_internal_list;

// tag-__pthread_mutex_s
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 92
struct __pthread_mutex_s;

// tag-bpf_insn
// file /usr/include/pcap/bpf.h line 108
struct bpf_insn;

// tag-bpf_program
// file /usr/include/pcap/bpf.h line 106
struct bpf_program;

// tag-challenge_t
// file ptunnel.h line 302
struct challenge_t;

// tag-ether_header
// file /usr/include/net/ethernet.h line 38
struct ether_header;

// tag-group
// file /usr/include/grp.h line 42
struct group;

// tag-hostent
// file /usr/include/netdb.h line 100
struct hostent;

// tag-in_addr
// file /usr/include/netinet/in.h line 31
struct in_addr;

// tag-md5_state_s
// file md5.h line 67
struct md5_state_s;

// tag-passwd
// file /usr/include/pwd.h line 49
struct passwd;

// tag-pcap
// file /usr/include/pcap/pcap.h line 79
struct pcap;

// tag-pcap_pkthdr
// file /usr/include/pcap/pcap.h line 160
struct pcap_pkthdr;

// tag-pqueue_elem_t
// file ptunnel.h line 361
struct pqueue_elem_t;

// tag-proxy_desc_t
// file ptunnel.h line 324
struct proxy_desc_t;

// tag-pthread_attr_t
// file /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h line 63
union pthread_attr_t;

// tag-sockaddr
// file /usr/include/x86_64-linux-gnu/bits/socket.h line 149
struct sockaddr;

// tag-sockaddr_in
// file /usr/include/netinet/in.h line 237
struct sockaddr_in;

// tag-timeval
// file /usr/include/x86_64-linux-gnu/bits/time.h line 30
struct timeval;

// tag-timezone
// file /usr/include/x86_64-linux-gnu/sys/time.h line 55
struct timezone;

// tag-xfer_stats_t
// file ptunnel.h line 312
struct xfer_stats_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

// __errno_location
// file /usr/include/x86_64-linux-gnu/bits/errno.h line 50
extern signed int * __errno_location(void);
// accept
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 243
extern signed int accept(signed int, struct sockaddr *, unsigned int *);
// atoi
// file /usr/include/stdlib.h line 147
extern signed int atoi(const char *);
// bind
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 123
extern signed int bind(signed int, struct sockaddr *, unsigned int);
// calc_icmp_checksum
// file ptunnel.c line 1499
unsigned short int calc_icmp_checksum(unsigned short int *data, signed int bytes);
// calloc
// file /usr/include/stdlib.h line 468
extern void * calloc(unsigned long int, unsigned long int);
// chdir
// file /usr/include/unistd.h line 497
extern signed int chdir(const char *);
// chroot
// file /usr/include/unistd.h line 944
extern signed int chroot(const char *);
// close
// file /usr/include/unistd.h line 353
extern signed int close(signed int);
// connect
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 137
extern signed int connect(signed int, struct sockaddr *, unsigned int);
// create_and_insert_proxy_desc
// file ptunnel.c line 1105
struct proxy_desc_t * create_and_insert_proxy_desc(unsigned short int id_no, unsigned short int icmp_id, signed int sock, struct sockaddr_in *addr, unsigned int dst_ip, unsigned int dst_port, unsigned int init_state, unsigned int type);
// create_fwd_desc
// file ptunnel.c line 1487
struct anonymous$7 * create_fwd_desc(unsigned short int seq_no, unsigned int data_len, char *data);
// exit
// file /usr/include/stdlib.h line 543
extern void exit(signed int);
// fclose
// file /usr/include/stdio.h line 237
extern signed int fclose(struct _IO_FILE *);
// fflush
// file /usr/include/stdio.h line 242
extern signed int fflush(struct _IO_FILE *);
// fopen
// file /usr/include/stdio.h line 272
extern struct _IO_FILE * fopen(const char *, const char *);
// fork
// file /usr/include/unistd.h line 756
extern signed int fork(void);
// fprintf
// file /usr/include/stdio.h line 356
extern signed int fprintf(struct _IO_FILE *, const char *, ...);
// free
// file /usr/include/stdlib.h line 483
extern void free(void *);
// freopen
// file /usr/include/stdio.h line 278
extern struct _IO_FILE * freopen(const char *, const char *, struct _IO_FILE *);
// generate_challenge
// file ptunnel.c line 1518
struct challenge_t * generate_challenge(void);
// generate_response
// file ptunnel.c line 1540
void generate_response(struct challenge_t *challenge);
// getgrnam
// file /usr/include/grp.h line 110
extern struct group * getgrnam(const char *);
// gethostbyname
// file /usr/include/netdb.h line 144
extern struct hostent * gethostbyname(const char *);
// getpid
// file /usr/include/unistd.h line 628
extern signed int getpid(void);
// getpwnam
// file /usr/include/pwd.h line 116
extern struct passwd * getpwnam(const char *);
// gettimeofday
// file /usr/include/x86_64-linux-gnu/sys/time.h line 71
extern signed int gettimeofday(struct timeval *, struct timezone *);
// handle_ack
// file ptunnel.c line 1423
void handle_ack(unsigned short int seq_no, struct anonymous$8 *ring, signed int *packets_awaiting_ack, signed int one_ack_only, signed int insert_idx, signed int *first_ack, unsigned short int *remote_ack, signed int is_pcap);
// handle_data
// file ptunnel.c line 1345
void handle_data(struct anonymous$6 *pkt, signed int total_len, struct anonymous$7 **ring, signed int *await_send, signed int *insert_idx, unsigned short int *next_expected_seq);
// handle_packet
// file ptunnel.c line 931
void handle_packet(char *buf, signed int bytes, signed int is_pcap, struct sockaddr_in *addr, signed int icmp_sock);
// htonl
// file /usr/include/netinet/in.h line 377
extern unsigned int htonl(unsigned int);
// htons
// file /usr/include/netinet/in.h line 379
extern unsigned short int htons(unsigned short int);
// inet_ntoa
// file /usr/include/arpa/inet.h line 53
extern char * inet_ntoa(struct in_addr);
// listen
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 233
extern signed int listen(signed int, signed int);
// malloc
// file /usr/include/stdlib.h line 466
extern void * malloc(unsigned long int);
// md5_append
// file md5.h line 82
void md5_append(struct md5_state_s *pms, const unsigned char *data, signed int nbytes);
// md5_finish
// file md5.h line 85
void md5_finish(struct md5_state_s *pms, unsigned char *digest);
// md5_init
// file md5.h line 79
void md5_init(struct md5_state_s *pms);
// md5_process
// file md5.c line 132
static void md5_process(struct md5_state_s *pms, const unsigned char *data);
// memcmp
// file /usr/include/string.h line 69
extern signed int memcmp(const void *, const void *, unsigned long int);
// memcpy
// file /usr/include/string.h line 46
extern void * memcpy(void *, const void *, unsigned long int);
// memset
// file /usr/include/string.h line 66
extern void * memset(void *, signed int, unsigned long int);
// ntohl
// file /usr/include/netinet/in.h line 374
extern unsigned int ntohl(unsigned int);
// ntohs
// file /usr/include/netinet/in.h line 375
extern unsigned short int ntohs(unsigned short int);
// openlog
// file /usr/include/x86_64-linux-gnu/sys/syslog.h line 181
extern void openlog(const char *, signed int, signed int);
// pcap_close
// file /usr/include/pcap/pcap.h line 371
void pcap_close(struct pcap *);
// pcap_compile
// file /usr/include/pcap/pcap.h line 389
signed int pcap_compile(struct pcap *, struct bpf_program *, const char *, signed int, unsigned int);
// pcap_dispatch
// file /usr/include/pcap/pcap.h line 373
signed int pcap_dispatch(struct pcap *, signed int, void (*)(unsigned char *, struct pcap_pkthdr *, const unsigned char *), unsigned char *);
// pcap_lookupnet
// file /usr/include/pcap/pcap.h line 275
signed int pcap_lookupnet(const char *, unsigned int *, unsigned int *, char *);
// pcap_open_live
// file /usr/include/pcap/pcap.h line 349
struct pcap * pcap_open_live(const char *, signed int, signed int, signed int, char *);
// pcap_packet_handler
// file ptunnel.c line 892
void pcap_packet_handler(unsigned char *refcon, struct pcap_pkthdr *hdr, const unsigned char *pkt);
// pcap_setfilter
// file /usr/include/pcap/pcap.h line 379
signed int pcap_setfilter(struct pcap *, struct bpf_program *);
// print_statistics
// file ptunnel.c line 860
void print_statistics(struct xfer_stats_t *xfer, signed int is_continuous);
// printf
// file /usr/include/stdio.h line 362
extern signed int printf(const char *, ...);
// pt_create_udp_socket
// file ptunnel.c line 542
signed int pt_create_udp_socket(signed int port);
// pt_forwarder
// file ptunnel.c line 467
void pt_forwarder(void);
// pt_log
// file ptunnel.c line 1579
void pt_log(signed int level, char *fmt, ...);
// pt_proxy
// file ptunnel.c line 578
void * pt_proxy(void *args);
// pthread_create
// file /usr/include/pthread.h line 235
extern signed int pthread_create(unsigned long int *, const union pthread_attr_t *, void * (*)(void *), void *);
// pthread_mutex_init
// file /usr/include/pthread.h line 751
extern signed int pthread_mutex_init(union anonymous$0 *, const union anonymous$4 *);
// pthread_mutex_lock
// file /usr/include/pthread.h line 764
extern signed int pthread_mutex_lock(union anonymous$0 *);
// pthread_mutex_unlock
// file /usr/include/pthread.h line 775
extern signed int pthread_mutex_unlock(union anonymous$0 *);
// queue_packet
// file ptunnel.c line 1205
signed int queue_packet(signed int icmp_sock, unsigned char type, char *buf, signed int num_bytes, unsigned short int id_no, unsigned short int icmp_id, unsigned short int *seq, struct anonymous$8 *ring, signed int *insert_idx, signed int *await_send, unsigned int ip, unsigned int port, unsigned int state, struct sockaddr_in *dest_addr, unsigned short int next_expected_seq, signed int *first_ack, unsigned short int *ping_seq);
// rand
// file /usr/include/stdlib.h line 374
extern signed int rand(void);
// recv
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 156
extern signed long int recv(signed int, void *, unsigned long int, signed int);
// recvfrom
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 174
extern signed long int recvfrom(signed int, void *, unsigned long int, signed int, struct sockaddr *, unsigned int *);
// remove_proxy_desc
// file ptunnel.c line 1162
void remove_proxy_desc(struct proxy_desc_t *cur, struct proxy_desc_t *prev);
// select
// file /usr/include/x86_64-linux-gnu/sys/select.h line 106
extern signed int select(signed int, struct anonymous *, struct anonymous *, struct anonymous *, struct timeval *);
// send
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 149
extern signed long int send(signed int, const void *, unsigned long int, signed int);
// send_packets
// file ptunnel.c line 1306
unsigned int send_packets(struct anonymous$7 **ring, signed int *xfer_idx, signed int *await_send, signed int *sock);
// send_termination_msg
// file ptunnel.c line 1571
void send_termination_msg(struct proxy_desc_t *cur, signed int icmp_sock);
// sendto
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 163
extern signed long int sendto(signed int, const void *, unsigned long int, signed int, struct sockaddr *, unsigned int);
// setcon
// file /usr/include/selinux/selinux.h line 40
extern signed int setcon(const char *);
// setgid
// file /usr/include/unistd.h line 717
extern signed int setgid(unsigned int);
// setsid
// file /usr/include/unistd.h line 667
extern signed int setsid(void);
// setsockopt
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 226
extern signed int setsockopt(signed int, signed int, signed int, const void *, unsigned int);
// setuid
// file /usr/include/unistd.h line 700
extern signed int setuid(unsigned int);
// signal
// file /usr/include/signal.h line 102
extern void (*signal(signed int, void (*)(signed int)))(signed int);
// snprintf
// file /usr/include/stdio.h line 386
extern signed int snprintf(char *, unsigned long int, const char *, ...);
// socket
// file /usr/include/x86_64-linux-gnu/sys/socket.h line 113
extern signed int socket(signed int, signed int, signed int);
// srand
// file /usr/include/stdlib.h line 376
extern void srand(unsigned int);
// strcmp
// file /usr/include/string.h line 144
extern signed int strcmp(const char *, const char *);
// strdup
// file /usr/include/string.h line 175
extern char * strdup(const char *);
// strerror
// file /usr/include/string.h line 412
extern char * strerror(signed int);
// strlen
// file /usr/include/string.h line 398
extern unsigned long int strlen(const char *);
// syslog
// file /usr/include/x86_64-linux-gnu/sys/syslog.h line 190
extern void syslog(signed int, const char *, ...);
// time
// file /usr/include/time.h line 192
extern signed long int time(signed long int *);
// time_as_double
// file ptunnel.c line 1615
double time_as_double(void);
// usage
// file ptunnel.c line 415
void usage(char *exec_name);
// validate_challenge
// file ptunnel.c line 1560
signed int validate_challenge(struct challenge_t *local, struct challenge_t *remote);
// vfprintf
// file /usr/include/stdio.h line 371
extern signed int vfprintf(struct _IO_FILE *, const char *, void **);
// vsnprintf
// file /usr/include/stdio.h line 390
extern signed int vsnprintf(char *, unsigned long int, const char *, void **);

struct bpf_program
{
  // bf_len
  unsigned int bf_len;
  // bf_insns
  struct bpf_insn *bf_insns;
};

struct anonymous$2
{
  // head
  struct pqueue_elem_t *head;
  // tail
  struct pqueue_elem_t *tail;
  // elems
  signed int elems;
};

struct anonymous$5
{
  // pcap_desc
  struct pcap *pcap_desc;
  // fp
  struct bpf_program fp;
  // netp
  unsigned int netp;
  // netmask
  unsigned int netmask;
  // pcap_err_buf
  char *pcap_err_buf;
  // pcap_data_buf
  char *pcap_data_buf;
  // pkt_q
  struct anonymous$2 pkt_q;
};

struct anonymous
{
  // __fds_bits
  signed long int __fds_bits[16l];
};

struct anonymous$8
{
  // pkt_len
  signed int pkt_len;
  // last_resend
  double last_resend;
  // resend_count
  signed int resend_count;
  // seq_no
  unsigned short int seq_no;
  // icmp_id
  unsigned short int icmp_id;
  // pkt
  struct anonymous$6 *pkt;
};

struct anonymous$7
{
  // seq_no
  signed int seq_no;
  // length
  signed int length;
  // remaining
  signed int remaining;
  // data
  char data[0l];
};

struct anonymous$1
{
  // magic
  unsigned int magic;
  // dst_ip
  unsigned int dst_ip;
  // dst_port
  unsigned int dst_port;
  // state
  unsigned int state;
  // ack
  unsigned int ack;
  // data_len
  unsigned int data_len;
  // seq_no
  unsigned short int seq_no;
  // id_no
  unsigned short int id_no;
  // data
  char data[0l];
} __attribute__ ((__packed__));

struct anonymous$6
{
  // type
  unsigned char type;
  // code
  unsigned char code;
  // checksum
  unsigned short int checksum;
  // identifier
  unsigned short int identifier;
  // seq
  unsigned short int seq;
  // data
  char data[0l];
} __attribute__ ((__packed__));

struct anonymous$3
{
  // vers_ihl
  unsigned char vers_ihl;
  // tos
  unsigned char tos;
  // pkt_len
  unsigned short int pkt_len;
  // id
  unsigned short int id;
  // flags_frag_offset
  unsigned short int flags_frag_offset;
  // ttl
  unsigned char ttl;
  // proto
  unsigned char proto;
  // checksum
  unsigned short int checksum;
  // src_ip
  unsigned int src_ip;
  // dst_ip
  unsigned int dst_ip;
  // data
  char data[0l];
} __attribute__ ((__packed__));

union anonymous$4
{
  // __size
  char __size[4l];
  // __align
  signed int __align;
};

struct __pthread_internal_list
{
  // __prev
  struct __pthread_internal_list *__prev;
  // __next
  struct __pthread_internal_list *__next;
};

struct __pthread_mutex_s
{
  // __lock
  signed int __lock;
  // __count
  unsigned int __count;
  // __owner
  signed int __owner;
  // __nusers
  unsigned int __nusers;
  // __kind
  signed int __kind;
  // __spins
  signed short int __spins;
  // __elision
  signed short int __elision;
  // __list
  struct __pthread_internal_list __list;
};

union anonymous$0
{
  // __data
  struct __pthread_mutex_s __data;
  // __size
  char __size[40l];
  // __align
  signed long int __align;
};

struct _IO_FILE
{
  // _flags
  signed int _flags;
  // _IO_read_ptr
  char *_IO_read_ptr;
  // _IO_read_end
  char *_IO_read_end;
  // _IO_read_base
  char *_IO_read_base;
  // _IO_write_base
  char *_IO_write_base;
  // _IO_write_ptr
  char *_IO_write_ptr;
  // _IO_write_end
  char *_IO_write_end;
  // _IO_buf_base
  char *_IO_buf_base;
  // _IO_buf_end
  char *_IO_buf_end;
  // _IO_save_base
  char *_IO_save_base;
  // _IO_backup_base
  char *_IO_backup_base;
  // _IO_save_end
  char *_IO_save_end;
  // _markers
  struct _IO_marker *_markers;
  // _chain
  struct _IO_FILE *_chain;
  // _fileno
  signed int _fileno;
  // _flags2
  signed int _flags2;
  // _old_offset
  signed long int _old_offset;
  // _cur_column
  unsigned short int _cur_column;
  // _vtable_offset
  signed char _vtable_offset;
  // _shortbuf
  char _shortbuf[1l];
  // _lock
  void *_lock;
  // _offset
  signed long int _offset;
  // __pad1
  void *__pad1;
  // __pad2
  void *__pad2;
  // __pad3
  void *__pad3;
  // __pad4
  void *__pad4;
  // __pad5
  unsigned long int __pad5;
  // _mode
  signed int _mode;
  // _unused2
  char _unused2[(signed long int)(sizeof(signed int) * 5) /*20l*/ ];
};

struct _IO_marker
{
  // _next
  struct _IO_marker *_next;
  // _sbuf
  struct _IO_FILE *_sbuf;
  // _pos
  signed int _pos;
};

struct bpf_insn
{
  // code
  unsigned short int code;
  // jt
  unsigned char jt;
  // jf
  unsigned char jf;
  // k
  unsigned int k;
};

struct challenge_t
{
  // sec
  unsigned int sec;
  // usec_rnd
  unsigned int usec_rnd;
  // random
  unsigned int random[6l];
} __attribute__ ((__packed__));

struct ether_header
{
  // ether_dhost
  unsigned char ether_dhost[6l];
  // ether_shost
  unsigned char ether_shost[6l];
  // ether_type
  unsigned short int ether_type;
} __attribute__ ((__packed__));

struct group
{
  // gr_name
  char *gr_name;
  // gr_passwd
  char *gr_passwd;
  // gr_gid
  unsigned int gr_gid;
  // gr_mem
  char **gr_mem;
};

struct hostent
{
  // h_name
  char *h_name;
  // h_aliases
  char **h_aliases;
  // h_addrtype
  signed int h_addrtype;
  // h_length
  signed int h_length;
  // h_addr_list
  char **h_addr_list;
};

struct in_addr
{
  // s_addr
  unsigned int s_addr;
};

struct md5_state_s
{
  // count
  unsigned int count[2l];
  // abcd
  unsigned int abcd[4l];
  // buf
  unsigned char buf[64l];
};

struct passwd
{
  // pw_name
  char *pw_name;
  // pw_passwd
  char *pw_passwd;
  // pw_uid
  unsigned int pw_uid;
  // pw_gid
  unsigned int pw_gid;
  // pw_gecos
  char *pw_gecos;
  // pw_dir
  char *pw_dir;
  // pw_shell
  char *pw_shell;
};

struct timeval
{
  // tv_sec
  signed long int tv_sec;
  // tv_usec
  signed long int tv_usec;
};

struct pcap_pkthdr
{
  // ts
  struct timeval ts;
  // caplen
  unsigned int caplen;
  // len
  unsigned int len;
};

struct pqueue_elem_t
{
  // bytes
  signed int bytes;
  // next
  struct pqueue_elem_t *next;
  // data
  char data[0l];
};

struct sockaddr
{
  // sa_family
  unsigned short int sa_family;
  // sa_data
  char sa_data[14l];
};

struct sockaddr_in
{
  // sin_family
  unsigned short int sin_family;
  // sin_port
  unsigned short int sin_port;
  // sin_addr
  struct in_addr sin_addr;
  // sin_zero
  unsigned char sin_zero[8l];
};

struct xfer_stats_t
{
  // bytes_in
  double bytes_in;
  // bytes_out
  double bytes_out;
  // icmp_in
  unsigned int icmp_in;
  // icmp_out
  unsigned int icmp_out;
  // icmp_resent
  unsigned int icmp_resent;
  // icmp_ack_out
  unsigned int icmp_ack_out;
};

struct proxy_desc_t
{
  // sock
  signed int sock;
  // bytes
  signed int bytes;
  // should_remove
  signed int should_remove;
  // buf
  char *buf;
  // id_no
  unsigned short int id_no;
  // my_seq
  unsigned short int my_seq;
  // ping_seq
  unsigned short int ping_seq;
  // next_remote_seq
  unsigned short int next_remote_seq;
  // pkt_type
  unsigned short int pkt_type;
  // remote_ack_val
  unsigned short int remote_ack_val;
  // icmp_id
  unsigned short int icmp_id;
  // recv_idx
  signed int recv_idx;
  // recv_xfer_idx
  signed int recv_xfer_idx;
  // send_idx
  signed int send_idx;
  // send_first_ack
  signed int send_first_ack;
  // recv_wait_send
  signed int recv_wait_send;
  // send_wait_ack
  signed int send_wait_ack;
  // next_resend_start
  signed int next_resend_start;
  // authenticated
  signed int authenticated;
  // challenge
  struct challenge_t *challenge;
  // state
  unsigned int state;
  // type_flag
  unsigned int type_flag;
  // dst_ip
  unsigned int dst_ip;
  // dst_port
  unsigned int dst_port;
  // dest_addr
  struct sockaddr_in dest_addr;
  // last_ack
  double last_ack;
  // last_activity
  double last_activity;
  // send_ring
  struct anonymous$8 send_ring[64l];
  // recv_ring
  struct anonymous$7 *recv_ring[64l];
  // xfer
  struct xfer_stats_t xfer;
  // next
  struct proxy_desc_t *next;
};

union pthread_attr_t
{
  // __size
  char __size[56l];
  // __align
  signed long int __align;
};

struct timezone
{
  // tz_minuteswest
  signed int tz_minuteswest;
  // tz_dsttime
  signed int tz_dsttime;
};


// chain
// file ptunnel.c line 115
struct proxy_desc_t *chain = ((struct proxy_desc_t *)NULL);
// chain_lock
// file ptunnel.c line 84
union anonymous$0 chain_lock;
// daemonize
// file ptunnel.c line 78
static char daemonize = (char)0;
// gid
// file ptunnel.c line 76
static unsigned int gid = (unsigned int)0;
// given_dst_ip
// file ptunnel.c line 103
unsigned int given_dst_ip = (unsigned int)0;
// given_proxy_ip
// file ptunnel.c line 102
unsigned int given_proxy_ip = (unsigned int)0;
// icmp_receive_buf_len
// file ptunnel.c line 110
const signed int icmp_receive_buf_len = (const signed int)((unsigned long int)(1024 + 20 + 8) + sizeof(struct anonymous$1) /*28ul*/ );
// log_file
// file ptunnel.c line 91
struct _IO_FILE *log_file = ((struct _IO_FILE *)NULL);
// log_level
// file ptunnel.c line 95
signed int log_level = 2;
// max_tunnels
// file ptunnel.c line 98
signed int max_tunnels = 10;
// mode
// file ptunnel.c line 96
signed int mode = 1;
// num_threads
// file ptunnel.c line 97
signed int num_threads = 0;
// num_threads_lock
// file ptunnel.c line 85
union anonymous$0 num_threads_lock;
// num_tunnels
// file ptunnel.c line 99
signed int num_tunnels = 0;
// password
// file ptunnel.c line 104
char *password = ((char *)NULL);
// password_digest
// file ptunnel.c line 105
char password_digest[16l];
// pcap
// file ptunnel.c line 88
char pcap = (char)0;
// pcap_buf_size
// file ptunnel.c line 111
const signed int pcap_buf_size = (const signed int)(((unsigned long int)(1024 + 20 + 8) + sizeof(struct anonymous$1) /*28ul*/  + (unsigned long int)64) * (unsigned long int)64);
// pcap_device
// file ptunnel.c line 106
char *pcap_device = ((char *)NULL);
// pcap_filter_program
// file ptunnel.c line 112
char pcap_filter_program[5l] = { 'i', 'c', 'm', 'p', 0 };
// pid_file
// file ptunnel.c line 79
static struct _IO_FILE *pid_file = (struct _IO_FILE *)(void *)0;
// print_stats
// file ptunnel.c line 89
char print_stats = (char)0;
// root_dir
// file ptunnel.c line 77
static char *root_dir = (char *)(void *)0;
// selinux_context
// file ptunnel.c line 73
static char *selinux_context = (char *)(void *)0;
// seq_expiry_tbl
// file ptunnel.c line 101
unsigned int *seq_expiry_tbl = ((unsigned int *)NULL);
// state_name
// file ptunnel.c line 116
const char *state_name[5l] = { "start", "ack", "data", "close", "authenticate" };
// stderr
// file /usr/include/stdio.h line 170
extern struct _IO_FILE *stderr;
// stdin
// file /usr/include/stdio.h line 168
extern struct _IO_FILE *stdin;
// stdout
// file /usr/include/stdio.h line 169
extern struct _IO_FILE *stdout;
// tcp_listen_port
// file ptunnel.c line 94
signed int tcp_listen_port = -1;
// tcp_port
// file ptunnel.c line 93
signed int tcp_port = -1;
// tcp_receive_buf_len
// file ptunnel.c line 109
const signed int tcp_receive_buf_len = 1024;
// uid
// file ptunnel.c line 75
static unsigned int uid = (unsigned int)0;
// unprivileged
// file ptunnel.c line 87
char unprivileged = (char)0;
// use_syslog
// file ptunnel.c line 90
char use_syslog = (char)0;
// use_udp
// file ptunnel.c line 100
signed int use_udp = 0;

// calc_icmp_checksum
// file ptunnel.c line 1499
unsigned short int calc_icmp_checksum(unsigned short int *data, signed int bytes)
{
  unsigned int sum;
  signed int i;
  sum = (unsigned int)0;
  i = 0;
  for( ; !(i >= bytes / 2); i = i + 1)
    sum = sum + (unsigned int)data[(signed long int)i];
  sum = (sum & (unsigned int)0xFFFF) + (sum >> 16);
  unsigned short int return_value_htons$1;
  return_value_htons$1=htons((unsigned short int)((unsigned int)0xFFFF - sum));
  sum = (unsigned int)return_value_htons$1;
  return (unsigned short int)sum;
}

// create_and_insert_proxy_desc
// file ptunnel.c line 1105
struct proxy_desc_t * create_and_insert_proxy_desc(unsigned short int id_no, unsigned short int icmp_id, signed int sock, struct sockaddr_in *addr, unsigned int dst_ip, unsigned int dst_port, unsigned int init_state, unsigned int type)
{
  struct proxy_desc_t *cur;
  pthread_mutex_lock(&chain_lock);
  if(num_tunnels >= max_tunnels)
  {
    pt_log(1, "Discarding incoming connection - too many tunnels! Maximum count is %d (adjust with the -m switch).\n", max_tunnels);
    if(!(sock == 0))
      close(sock);

    pthread_mutex_unlock(&chain_lock);
    return ((struct proxy_desc_t *)NULL);
  }

  else
  {
    num_tunnels = num_tunnels + 1;
    pthread_mutex_unlock(&chain_lock);
    pt_log(4, "Adding proxy desc to run loop. Type is %s. Will create socket: %s\n", type == (unsigned int)1073741824 ? "user" : "proxy", sock != 0 ? "No" : "Yes");
    void *return_value_calloc$1;
    return_value_calloc$1=calloc((unsigned long int)1, sizeof(struct proxy_desc_t) /*2728ul*/ );
    cur = (struct proxy_desc_t *)return_value_calloc$1;
    cur->id_no = id_no;
    cur->dest_addr = *addr;
    cur->dst_ip = dst_ip;
    cur->dst_port = dst_port;
    cur->icmp_id = icmp_id;
    if(sock == 0)
    {
      cur->sock=socket(2, 1, 0);
      memset((void *)addr, 0, sizeof(struct sockaddr_in) /*16ul*/ );
      addr->sin_port=htons((unsigned short int)dst_port);
      addr->sin_addr.s_addr = dst_ip;
      addr->sin_family = (unsigned short int)2;
      signed int return_value_connect$6;
      return_value_connect$6=connect(cur->sock, (struct sockaddr *)addr, (unsigned int)sizeof(struct sockaddr_in) /*16ul*/ );
      if(!(return_value_connect$6 >= 0))
      {
        char *return_value_inet_ntoa$2;
        return_value_inet_ntoa$2=inet_ntoa(*((struct in_addr *)&addr->sin_addr.s_addr));
        unsigned short int return_value_ntohs$3;
        return_value_ntohs$3=ntohs(addr->sin_port);
        signed int *return_value___errno_location$4;
        return_value___errno_location$4=__errno_location();
        char *return_value_strerror$5;
        return_value_strerror$5=strerror(*return_value___errno_location$4);
        pt_log(0, "Connect to %s:%d failed: %s\n", return_value_inet_ntoa$2, return_value_ntohs$3, return_value_strerror$5);
      }

    }

    else
      cur->sock = sock;
    cur->state = init_state;
    cur->type_flag = type;
    if(cur->type_flag == 1073741824u)
      cur->pkt_type = (unsigned short int)8;

    else
      cur->pkt_type = (unsigned short int)(unprivileged != 0 ? 8 : 0);
    void *return_value_malloc$7;
    return_value_malloc$7=malloc((unsigned long int)icmp_receive_buf_len);
    cur->buf = (char *)return_value_malloc$7;
    cur->last_activity=time_as_double();
    cur->authenticated = 0;
    pthread_mutex_lock(&chain_lock);
    cur->next = chain;
    chain = cur;
    pthread_mutex_unlock(&chain_lock);
    cur->xfer.bytes_in = 0.0;
    cur->xfer.bytes_out = 0.0;
    return cur;
  }
}

// create_fwd_desc
// file ptunnel.c line 1487
struct anonymous$7 * create_fwd_desc(unsigned short int seq_no, unsigned int data_len, char *data)
{
  struct anonymous$7 *fwd_desc;
  void *return_value_malloc$1;
  return_value_malloc$1=malloc(sizeof(struct anonymous$7) /*12ul*/  + (unsigned long int)data_len);
  fwd_desc = (struct anonymous$7 *)return_value_malloc$1;
  fwd_desc->seq_no = (signed int)seq_no;
  fwd_desc->length = (signed int)data_len;
  fwd_desc->remaining = (signed int)data_len;
  if(data_len >= 1u)
    memcpy((void *)fwd_desc->data, (const void *)data, (unsigned long int)data_len);

  return fwd_desc;
}

// generate_challenge
// file ptunnel.c line 1518
struct challenge_t * generate_challenge(void)
{
  struct timeval tt;
  struct challenge_t *c;
  signed int i;
  void *return_value_calloc$1;
  return_value_calloc$1=calloc((unsigned long int)1, sizeof(struct challenge_t) /*32ul*/ );
  c = (struct challenge_t *)return_value_calloc$1;
  gettimeofday(&tt, ((struct timezone *)NULL));
  c->sec = (unsigned int)tt.tv_sec;
  signed int return_value_rand$2;
  return_value_rand$2=rand();
  c->usec_rnd = (unsigned int)(tt.tv_usec + (signed long int)return_value_rand$2);
  i = 0;
  signed int return_value_rand$3;
  for( ; !(i >= 6); i = i + 1)
  {
    return_value_rand$3=rand();
    c->random[(signed long int)i] = (unsigned int)return_value_rand$3;
  }
  return c;
}

// generate_response
// file ptunnel.c line 1540
void generate_response(struct challenge_t *challenge)
{
  unsigned char *buf;
  struct md5_state_s state;
  void *return_value_malloc$1;
  return_value_malloc$1=malloc(sizeof(struct challenge_t) /*32ul*/  + (unsigned long int)16);
  buf = (unsigned char *)return_value_malloc$1;
  memcpy((void *)buf, (const void *)challenge, sizeof(struct challenge_t) /*32ul*/ );
  memcpy((void *)&buf[(signed long int)sizeof(struct challenge_t) /*32ul*/ ], (const void *)password_digest, (unsigned long int)16);
  memset((void *)challenge, 0, sizeof(struct challenge_t) /*32ul*/ );
  md5_init(&state);
  md5_append(&state, buf, (signed int)(sizeof(struct challenge_t) /*32ul*/  + (unsigned long int)16));
  md5_finish(&state, (unsigned char *)challenge);
}

// handle_ack
// file ptunnel.c line 1423
void handle_ack(unsigned short int seq_no, struct anonymous$8 *ring, signed int *packets_awaiting_ack, signed int one_ack_only, signed int insert_idx, signed int *first_ack, unsigned short int *remote_ack, signed int is_pcap)
{
  signed int i;
  signed int j;
  signed int k;
  struct anonymous$1 *pt_pkt;
  if(*packets_awaiting_ack >= 1)
  {
    if(!(one_ack_only == 0))
    {
      i = 0;
      for( ; !(i >= 64); i = i + 1)
        if(!((ring + (signed long int)i)->pkt == ((struct anonymous$6 *)NULL)))
        {
          if((ring + (signed long int)i)->seq_no == seq_no)
          {
            if(is_pcap == 0)
            {
              pt_log(4, "Received ack for only seq %d\n", seq_no);
              pt_pkt = (struct anonymous$1 *)(ring + (signed long int)i)->pkt->data;
              unsigned int return_value_ntohl$1;
              return_value_ntohl$1=ntohl(pt_pkt->ack);
              *remote_ack = (unsigned short int)return_value_ntohl$1;
              free((void *)(ring + (signed long int)i)->pkt);
              (ring + (signed long int)i)->pkt = ((struct anonymous$6 *)NULL);
              *packets_awaiting_ack = *packets_awaiting_ack - 1;
              if(i == *first_ack)
              {
                j = 1;
                for( ; !(j >= 64); j = j + 1)
                {
                  k = (i + j) % 64;
                  if(!((ring + (signed long int)k)->pkt == ((struct anonymous$6 *)NULL)))
                  {
                    *first_ack = k;
                    break;
                  }

                  if(k == i)
                    *first_ack = insert_idx;

                  j = j + 1;
                }
              }

              goto __CPROVER_DUMP_L17;
            }

          }

        }

    }

    else
    {
      signed int handle_ack$$1$$1$$2$$i;
      signed int can_ack = 0;
      signed int count = 0;
      handle_ack$$1$$1$$2$$i = insert_idx - 1;
      if(!(handle_ack$$1$$1$$2$$i >= 0))
        handle_ack$$1$$1$$2$$i = 64 - 1;

      pt_log(4, "Received ack-series starting at seq %d\n", seq_no);
      for( ; !(count >= 64); count = count + 1)
      {
        if((ring + (signed long int)handle_ack$$1$$1$$2$$i)->pkt == ((struct anonymous$6 *)NULL))
          break;

        if((ring + (signed long int)handle_ack$$1$$1$$2$$i)->seq_no == seq_no)
          can_ack = 1;

        else
          if(can_ack == 0)
            *first_ack = handle_ack$$1$$1$$2$$i;

        if(!(can_ack == 0))
        {
          free((void *)(ring + (signed long int)handle_ack$$1$$1$$2$$i)->pkt);
          (ring + (signed long int)handle_ack$$1$$1$$2$$i)->pkt = ((struct anonymous$6 *)NULL);
          *packets_awaiting_ack = *packets_awaiting_ack - 1;
        }

        handle_ack$$1$$1$$2$$i = handle_ack$$1$$1$$2$$i - 1;
        if(!(handle_ack$$1$$1$$2$$i >= 0))
          handle_ack$$1$$1$$2$$i = 64 - 1;

      }
    }
  }


__CPROVER_DUMP_L17:
  ;
}

// handle_data
// file ptunnel.c line 1345
void handle_data(struct anonymous$6 *pkt, signed int total_len, struct anonymous$7 **ring, signed int *await_send, signed int *insert_idx, unsigned short int *next_expected_seq)
{
  struct anonymous$1 *pt_pkt = (struct anonymous$1 *)pkt->data;
  signed int expected_len = (signed int)(sizeof(struct anonymous$3) /*20ul*/  + sizeof(struct anonymous$6) /*8ul*/  + sizeof(struct anonymous$1) /*28ul*/ );
  expected_len = expected_len + (signed int)pt_pkt->data_len;
  expected_len = expected_len + expected_len % 2;
  if(!(use_udp == 0))
    expected_len = expected_len - (signed int)sizeof(struct anonymous$3) /*20ul*/ ;

  if(!(total_len >= expected_len))
  {
    pt_log(0, "Packet not completely received: %d Should be: %d. For some reason, this error is fatal.\n", total_len, expected_len);
    pt_log(4, "Data length: %d Total length: %d\n", pt_pkt->data_len, total_len);
    exit(0);
  }

  if(pt_pkt->seq_no == *next_expected_seq)
  {
    if(ring[(signed long int)*insert_idx] == ((struct anonymous$7 *)NULL))
    {
      ring[(signed long int)*insert_idx]=create_fwd_desc(pt_pkt->seq_no, pt_pkt->data_len, pt_pkt->data);
      *await_send = *await_send + 1;
      *insert_idx = *insert_idx + 1;
    }

    else
      if(!(ring[(signed long int)*insert_idx] == ((struct anonymous$7 *)NULL)))
        pt_log(4, "Dup packet?\n");

    *next_expected_seq = *next_expected_seq + 1;
    if(*insert_idx >= 64)
      *insert_idx = 0;

    while(!(ring[(signed long int)*insert_idx] == ((struct anonymous$7 *)NULL)))
      if(ring[(signed long int)*insert_idx]->seq_no == (signed int)*next_expected_seq)
      {
        *next_expected_seq = *next_expected_seq + 1;
        *insert_idx = *insert_idx + 1;
        if(*insert_idx >= 64)
          *insert_idx = 0;

      }

      else
        break;
  }

  else
  {
    signed int r;
    signed int s;
    signed int d;
    signed int pos = -1;
    r = (signed int)*next_expected_seq;
    s = (signed int)pt_pkt->seq_no;
    d = s - r;
    if(!(d >= 0))
    {
      d = (s + 0xFFFF) - r;
      if(!(d >= 64))
        pos = (*insert_idx + d) % 64;

    }

    else
      if(!(d >= 64))
        pos = (*insert_idx + d) % 64;

    if(!(pos == -1))
    {
      if(ring[(signed long int)pos] == ((struct anonymous$7 *)NULL))
      {
        pt_log(3, "Out of order. Expected: %d  Got: %d  Inserted: %d (cur = %d)\n", *next_expected_seq, pt_pkt->seq_no, pos, *insert_idx);
        ring[(signed long int)pos]=create_fwd_desc(pt_pkt->seq_no, pt_pkt->data_len, pt_pkt->data);
        *await_send = *await_send + 1;
      }

    }

  }
}

// handle_packet
// file ptunnel.c line 931
void handle_packet(char *buf, signed int bytes, signed int is_pcap, struct sockaddr_in *addr, signed int icmp_sock)
{
  struct anonymous$3 *ip_pkt;
  struct anonymous$6 *pkt;
  struct anonymous$1 *pt_pkt;
  struct proxy_desc_t *cur;
  unsigned int type_flag;
  unsigned int pkt_flag;
  unsigned int init_state;
  struct challenge_t *challenge;
  struct timeval tt;
  _Bool tmp_if_expr$6;
  _Bool tmp_if_expr$9;
  _Bool tmp_if_expr$8;
  unsigned int return_value_ntohl$7;
  _Bool tmp_if_expr$12;
  _Bool tmp_if_expr$13;
  _Bool tmp_if_expr$14;
  if(!((unsigned long int)bytes >= sizeof(struct anonymous$6) * 4 + 4 /*36ul*/ ))
    pt_log(3, "Skipping this packet - too short. Expect: %d+%d = %d ; Got: %d\n", sizeof(struct anonymous$6) /*8ul*/ , sizeof(struct anonymous$1) /*28ul*/ , sizeof(struct anonymous$6) /*8ul*/  + sizeof(struct anonymous$1) /*28ul*/ , bytes);

  else
  {
    if(!(use_udp == 0))
    {
      ip_pkt = ((struct anonymous$3 *)NULL);
      pkt = (struct anonymous$6 *)buf;
      pt_pkt = (struct anonymous$1 *)pkt->data;
    }

    else
    {
      ip_pkt = (struct anonymous$3 *)buf;
      pkt = (struct anonymous$6 *)ip_pkt->data;
      pt_pkt = (struct anonymous$1 *)pkt->data;
    }
    unsigned int return_value_ntohl$15;
    return_value_ntohl$15=ntohl(pt_pkt->magic);
    if(return_value_ntohl$15 == 0xD5200880)
    {
      pt_pkt->state=ntohl(pt_pkt->state);
      pkt->identifier=ntohs(pkt->identifier);
      pt_pkt->id_no=ntohs(pt_pkt->id_no);
      pt_pkt->seq_no=ntohs(pt_pkt->seq_no);
      pthread_mutex_lock(&chain_lock);
      cur = chain;
      for( ; !(cur == ((struct proxy_desc_t *)NULL)); cur = cur->next)
        if(cur->id_no == pt_pkt->id_no)
          break;

      pthread_mutex_unlock(&chain_lock);
      if(!(cur == ((struct proxy_desc_t *)NULL)))
      {
        type_flag = cur->type_flag;
        if(type_flag == 2147483648u)
          cur->icmp_id = pkt->identifier;

        if(is_pcap == 0)
          cur->xfer.icmp_in = cur->xfer.icmp_in + 1u;

      }

      else
        type_flag = (unsigned int)-2147483648;
      pkt_flag = pt_pkt->state & (unsigned int)-1073741824;
      pt_pkt->state = pt_pkt->state & (unsigned int)~-1073741824;
      unsigned int return_value_ntohl$1;
      return_value_ntohl$1=ntohl(pt_pkt->data_len);
      unsigned int return_value_ntohl$2;
      return_value_ntohl$2=ntohl(pt_pkt->ack);
      pt_log(5, "Recv: %d [%d] bytes [seq = %d] [type = %s] [ack = %d] [icmp = %d] [user = %s] [pcap = %d]\n", bytes, return_value_ntohl$1, pt_pkt->seq_no, state_name[(signed long int)(pt_pkt->state & (unsigned int)~-1073741824)], return_value_ntohl$2, pkt->type, pkt_flag == (unsigned int)1073741824 ? "yes" : "no", is_pcap);
      if(pkt_flag == 1073741824u && type_flag == 2147483648u || pkt_flag == 2147483648u && type_flag == 1073741824u)
      {
        pt_pkt->data_len=ntohl(pt_pkt->data_len);
        pt_pkt->ack=ntohl(pt_pkt->ack);
        if(pt_pkt->state == 0u)
        {
          if(cur == ((struct proxy_desc_t *)NULL) && type_flag == 2147483648u)
          {
            char *return_value_inet_ntoa$3;
            return_value_inet_ntoa$3=inet_ntoa(*((struct in_addr *)&addr->sin_addr));
            pt_log(1, "Incoming tunnel request from %s.\n", return_value_inet_ntoa$3);
            gettimeofday(&tt, ((struct timezone *)NULL));
            if(!(tt.tv_sec >= (signed long int)seq_expiry_tbl[(signed long int)pt_pkt->id_no]))
            {
              pt_log(3, "Dropping request: ID was recently in use.\n");
              goto __CPROVER_DUMP_L44;
            }

            char *return_value_inet_ntoa$4;
            return_value_inet_ntoa$4=inet_ntoa(*((struct in_addr *)&pt_pkt->dst_ip));
            unsigned int return_value_ntohl$5;
            return_value_ntohl$5=ntohl(pt_pkt->dst_port);
            pt_log(1, "Starting new session to %s:%d with ID %d\n", return_value_inet_ntoa$4, return_value_ntohl$5, pt_pkt->id_no);
            if(!(given_dst_ip == 0u))
              tmp_if_expr$6 = given_dst_ip != pt_pkt->dst_ip ? (_Bool)1 : (_Bool)0;

            else
              tmp_if_expr$6 = (_Bool)0;
            if(tmp_if_expr$6)
              tmp_if_expr$9 = (_Bool)1;

            else
            {
              if(!(tcp_port == -1))
              {
                return_value_ntohl$7=ntohl(pt_pkt->dst_port);
                tmp_if_expr$8 = (unsigned int)tcp_port != return_value_ntohl$7 ? (_Bool)1 : (_Bool)0;
              }

              else
                tmp_if_expr$8 = (_Bool)0;
              tmp_if_expr$9 = tmp_if_expr$8 ? (_Bool)1 : (_Bool)0;
            }
            if(tmp_if_expr$9)
            {
              pt_log(1, "Destination administratively prohibited!\n");
              goto __CPROVER_DUMP_L44;
            }

            if(!(password == ((char *)NULL)))
              init_state = (unsigned int)4;

            else
              init_state = (unsigned int)2;
            unsigned int return_value_ntohl$10;
            return_value_ntohl$10=ntohl(pt_pkt->dst_port);
            cur=create_and_insert_proxy_desc(pt_pkt->id_no, pkt->identifier, 0, addr, pt_pkt->dst_ip, return_value_ntohl$10, init_state, (unsigned int)-2147483648);
            if(init_state == 4u)
            {
              pt_log(4, "Sending authentication challenge..\n");
              cur->challenge=generate_challenge();
              memcpy((void *)cur->buf, (const void *)cur->challenge, sizeof(struct challenge_t) /*32ul*/ );
              queue_packet(icmp_sock, (unsigned char)cur->pkt_type, cur->buf, (signed int)sizeof(struct challenge_t) /*32ul*/ , cur->id_no, cur->icmp_id, &cur->my_seq, cur->send_ring, &cur->send_idx, &cur->send_wait_ack, (unsigned int)0, (unsigned int)0, (unsigned int)4 | cur->type_flag, &cur->dest_addr, cur->next_remote_seq, &cur->send_first_ack, &cur->ping_seq);
            }

          }

          else
            if(type_flag == 1073741824u)
            {
              pt_log(0, "Dropping proxy session request - we are not a proxy!\n");
              goto __CPROVER_DUMP_L44;
            }

            else
              pt_log(0, "Dropping duplicate proxy session request.\n");
        }

        else
          if(!(cur == ((struct proxy_desc_t *)NULL)))
          {
            if(pt_pkt->state == 4u)
            {
              if(!((unsigned long int)pt_pkt->data_len == sizeof(struct challenge_t) /*32ul*/ ))
              {
                pt_log(0, "Received challenge packet, but data length is not as expected.\n");
                pt_log(4, "Data length: %d  Expected: %d\n", pt_pkt->data_len, sizeof(struct challenge_t) /*32ul*/ );
                cur->should_remove = 1;
                goto __CPROVER_DUMP_L44;
              }

              pt_pkt->data_len = (unsigned int)0;
              challenge = (struct challenge_t *)pt_pkt->data;
              if(type_flag == 1073741824u)
              {
                if(password == ((char *)NULL))
                {
                  pt_log(0, "This proxy requires a password! Please supply one using the -x switch.\n");
                  send_termination_msg(cur, icmp_sock);
                  cur->should_remove = 1;
                  goto __CPROVER_DUMP_L44;
                }

                pt_log(4, "Got authentication challenge - sending response\n");
                generate_response(challenge);
                queue_packet(icmp_sock, (unsigned char)cur->pkt_type, (char *)challenge, (signed int)sizeof(struct challenge_t) /*32ul*/ , cur->id_no, cur->icmp_id, &cur->my_seq, cur->send_ring, &cur->send_idx, &cur->send_wait_ack, (unsigned int)0, (unsigned int)0, (unsigned int)4 | cur->type_flag, &cur->dest_addr, cur->next_remote_seq, &cur->send_first_ack, &cur->ping_seq);
                cur->authenticated = 1;
                handle_data(pkt, bytes, cur->recv_ring, &cur->recv_wait_send, &cur->recv_idx, &cur->next_remote_seq);
                goto __CPROVER_DUMP_L44;
              }

              else
                if(type_flag == 2147483648u)
                {
                  pt_log(4, "Received remote challenge response.\n");
                  signed int return_value_validate_challenge$11;
                  return_value_validate_challenge$11=validate_challenge(cur->challenge, challenge);
                  if(!(return_value_validate_challenge$11 == 0))
                    tmp_if_expr$12 = (_Bool)1;

                  else
                    tmp_if_expr$12 = cur->authenticated != 0 ? (_Bool)1 : (_Bool)0;
                  if(tmp_if_expr$12)
                  {
                    pt_log(3, "Remote end authenticated successfully.\n");
                    cur->authenticated = 1;
                    cur->state = (unsigned int)2;
                    handle_data(pkt, bytes, cur->recv_ring, &cur->recv_wait_send, &cur->recv_idx, &cur->next_remote_seq);
                  }

                  else
                  {
                    pt_log(1, "Remote end failed authentication.\n");
                    send_termination_msg(cur, icmp_sock);
                    cur->should_remove = 1;
                  }
                  goto __CPROVER_DUMP_L44;
                }

            }

          }

        if(!(cur == ((struct proxy_desc_t *)NULL)))
        {
          if(pt_pkt->state == 3u)
          {
            pt_log(1, "Received session close from remote peer.\n");
            cur->should_remove = 1;
            goto __CPROVER_DUMP_L44;
          }

        }

        if(type_flag == 2147483648u && !(password == ((char *)NULL)) && !(cur == ((struct proxy_desc_t *)NULL)))
        {
          if(cur->authenticated == 0)
          {
            pt_log(4, "Ignoring packet with seq-no %d - not authenticated yet.\n", pt_pkt->seq_no);
            goto __CPROVER_DUMP_L44;
          }

        }

        if(!(cur == ((struct proxy_desc_t *)NULL)))
        {
          if(!(cur->sock == 0))
          {
            if(pt_pkt->state == 2u)
              tmp_if_expr$13 = (_Bool)1;

            else
              tmp_if_expr$13 = pt_pkt->state == (unsigned int)0 ? (_Bool)1 : (_Bool)0;
            if(tmp_if_expr$13)
              tmp_if_expr$14 = (_Bool)1;

            else
              tmp_if_expr$14 = pt_pkt->state == (unsigned int)1 ? (_Bool)1 : (_Bool)0;
            if(tmp_if_expr$14)
              handle_data(pkt, bytes, cur->recv_ring, &cur->recv_wait_send, &cur->recv_idx, &cur->next_remote_seq);

            handle_ack((unsigned short int)pt_pkt->ack, cur->send_ring, &cur->send_wait_ack, 0, cur->send_idx, &cur->send_first_ack, &cur->remote_ack_val, is_pcap);
            cur->last_activity=time_as_double();
          }

        }

      }

    }

    else
      pt_log(3, "Ignored incoming packet.\n");
  }

__CPROVER_DUMP_L44:
  ;
}

// main
// file ptunnel.c line 119
signed int main(signed int argc, char **argv)
{
  signed int i;
  signed int opt;
  struct md5_state_s state;
  struct hostent *host_ent;
  struct passwd *pwnam;
  struct group *grnam;
  signed int pid;
  signed long int return_value_time$1;
  return_value_time$1=time(((signed long int *)NULL));
  srand((unsigned int)return_value_time$1);
  memset((void *)password_digest, 0, (unsigned long int)16);
  void *return_value_calloc$2;
  return_value_calloc$2=calloc((unsigned long int)65536, sizeof(unsigned int) /*4ul*/ );
  seq_expiry_tbl = (unsigned int *)return_value_calloc$2;
  log_file = stdout;
  opt = 0;
  mode = 1;
  i = 1;
  signed int return_value_strcmp$35;
  signed int return_value_strcmp$34;
  signed int return_value_strcmp$33;
  signed int return_value_strcmp$32;
  signed int return_value_strcmp$31;
  signed int return_value_strcmp$30;
  signed int return_value_strcmp$29;
  signed int return_value_strcmp$28;
  signed int return_value_strcmp$27;
  signed int return_value_strcmp$26;
  signed int return_value_strcmp$25;
  signed int return_value_strcmp$24;
  signed int return_value_strcmp$23;
  signed int return_value_strcmp$22;
  signed int return_value_strcmp$21;
  signed int return_value_strcmp$20;
  signed int return_value_strcmp$19;
  unsigned long int return_value_strlen$3;
  unsigned long int return_value_strlen$4;
  signed int *return_value___errno_location$7;
  char *tmp_if_expr$11;
  signed int *return_value___errno_location$9;
  char *return_value_strerror$10;
  signed int *return_value___errno_location$12;
  char *tmp_if_expr$16;
  signed int *return_value___errno_location$14;
  char *return_value_strerror$15;
  signed int *return_value___errno_location$17;
  char *return_value_strerror$18;
  for( ; !(i >= argc); i = i + 1)
  {
    signed int return_value_strcmp$36;
    return_value_strcmp$36=strcmp(argv[(signed long int)i], "-p");
    if(return_value_strcmp$36 == 0)
    {
      mode = 0;
      opt = 1;
    }

    else
    {
      return_value_strcmp$35=strcmp(argv[(signed long int)i], "-x");
      if(return_value_strcmp$35 == 0)
        opt = 3;

      else
      {
        return_value_strcmp$34=strcmp(argv[(signed long int)i], "-lp");
        if(return_value_strcmp$34 == 0)
          opt = 4;

        else
        {
          return_value_strcmp$33=strcmp(argv[(signed long int)i], "-da");
          if(return_value_strcmp$33 == 0)
            opt = 5;

          else
          {
            return_value_strcmp$32=strcmp(argv[(signed long int)i], "-dp");
            if(return_value_strcmp$32 == 0)
              opt = 6;

            else
            {
              return_value_strcmp$31=strcmp(argv[(signed long int)i], "-v");
              if(return_value_strcmp$31 == 0)
                opt = 7;

              else
              {
                return_value_strcmp$30=strcmp(argv[(signed long int)i], "-m");
                if(return_value_strcmp$30 == 0)
                  opt = 8;

                else
                {
                  return_value_strcmp$29=strcmp(argv[(signed long int)i], "-u");
                  if(return_value_strcmp$29 == 0)
                    unprivileged = (char)!(unprivileged != 0);

                  else
                  {
                    return_value_strcmp$28=strcmp(argv[(signed long int)i], "-c");
                    if(return_value_strcmp$28 == 0)
                      opt = 10;

                    else
                    {
                      return_value_strcmp$27=strcmp(argv[(signed long int)i], "-f");
                      if(return_value_strcmp$27 == 0)
                        opt = 11;

                      else
                      {
                        return_value_strcmp$26=strcmp(argv[(signed long int)i], "-s");
                        if(return_value_strcmp$26 == 0)
                          print_stats = (char)!(print_stats != 0);

                        else
                        {
                          return_value_strcmp$25=strcmp(argv[(signed long int)i], "-syslog");
                          if(return_value_strcmp$25 == 0)
                            use_syslog = (char)!(use_syslog != 0);

                          else
                          {
                            return_value_strcmp$24=strcmp(argv[(signed long int)i], "-setuid");
                            if(return_value_strcmp$24 == 0)
                              opt = 12;

                            else
                            {
                              return_value_strcmp$23=strcmp(argv[(signed long int)i], "-setgid");
                              if(return_value_strcmp$23 == 0)
                                opt = 13;

                              else
                              {
                                return_value_strcmp$22=strcmp(argv[(signed long int)i], "-chroot");
                                if(return_value_strcmp$22 == 0)
                                  opt = 14;

                                else
                                {
                                  return_value_strcmp$21=strcmp(argv[(signed long int)i], "-setcon");
                                  if(return_value_strcmp$21 == 0)
                                    opt = 15;

                                  else
                                  {
                                    return_value_strcmp$20=strcmp(argv[(signed long int)i], "-daemon");
                                    if(return_value_strcmp$20 == 0)
                                      opt = 16;

                                    else
                                    {
                                      return_value_strcmp$19=strcmp(argv[(signed long int)i], "-udp");
                                      if(return_value_strcmp$19 == 0)
                                        use_udp = 1;

                                      else
                                        switch(opt)
                                        {
                                          case 1:
                                          {
                                            host_ent=gethostbyname(argv[(signed long int)i]);
                                            if(host_ent == ((struct hostent *)NULL))
                                            {
                                              pt_log(0, "Failed to look up %s as proxy address\n", argv[(signed long int)i]);
                                              return 1;
                                            }

                                            given_proxy_ip = *((unsigned int *)host_ent->h_addr_list[(signed long int)0]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 3:
                                          {
                                            password = argv[(signed long int)i];
                                            pt_log(4, "Password set - unauthenicated connections will be refused.\n");
                                            md5_init(&state);
                                            return_value_strlen$3=strlen(password);
                                            md5_append(&state, (unsigned char *)password, (signed int)return_value_strlen$3);
                                            md5_finish(&state, (unsigned char *)password_digest);
                                            return_value_strlen$4=strlen(argv[(signed long int)i]);
                                            memset((void *)argv[(signed long int)i], 32, return_value_strlen$4);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 4:
                                          {
                                            tcp_listen_port=atoi(argv[(signed long int)i]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 5:
                                          {
                                            host_ent=gethostbyname(argv[(signed long int)i]);
                                            if(host_ent == ((struct hostent *)NULL))
                                            {
                                              pt_log(0, "Failed to look up %s as destination address\n", argv[(signed long int)i]);
                                              return 1;
                                            }

                                            given_dst_ip = *((unsigned int *)host_ent->h_addr_list[(signed long int)0]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 6:
                                          {
                                            tcp_port=atoi(argv[(signed long int)i]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 8:
                                          {
                                            max_tunnels=atoi(argv[(signed long int)i]);
                                            if(!(max_tunnels >= 1))
                                              max_tunnels = 10;

                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 7:
                                          {
                                            log_level=atoi(argv[(signed long int)i]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 10:
                                          {
                                            pcap_device = argv[(signed long int)i];
                                            pcap = (char)1;
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 11:
                                          {
                                            log_file=fopen(argv[(signed long int)i], "a");
                                            if(log_file == ((struct _IO_FILE *)NULL))
                                            {
                                              log_file = stdout;
                                              signed int *return_value___errno_location$5;
                                              return_value___errno_location$5=__errno_location();
                                              char *return_value_strerror$6;
                                              return_value_strerror$6=strerror(*return_value___errno_location$5);
                                              pt_log(0, "Failed to open log file: '%s'. Cause: %s\n", argv[(signed long int)i], return_value_strerror$6);
                                              pt_log(0, "Reverting log to standard out.\n");
                                            }

                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 12:
                                          {
                                            return_value___errno_location$7=__errno_location();
                                            *return_value___errno_location$7 = 0;
                                            pwnam=getpwnam(argv[(signed long int)i]);
                                            if(pwnam == ((struct passwd *)NULL))
                                            {
                                              signed int *return_value___errno_location$8;
                                              return_value___errno_location$8=__errno_location();
                                              if(!(*return_value___errno_location$8 == 0))
                                              {
                                                return_value___errno_location$9=__errno_location();
                                                return_value_strerror$10=strerror(*return_value___errno_location$9);
                                                tmp_if_expr$11 = return_value_strerror$10;
                                              }

                                              else
                                                tmp_if_expr$11 = "unknown user";
                                              pt_log(0, "%s: %s\n", argv[(signed long int)i], tmp_if_expr$11);
                                              exit(1);
                                            }

                                            uid = pwnam->pw_uid;
                                            if(gid == 0u)
                                              gid = pwnam->pw_gid;

                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 13:
                                          {
                                            return_value___errno_location$12=__errno_location();
                                            *return_value___errno_location$12 = 0;
                                            grnam=getgrnam(argv[(signed long int)i]);
                                            if(grnam == ((struct group *)NULL))
                                            {
                                              signed int *return_value___errno_location$13;
                                              return_value___errno_location$13=__errno_location();
                                              if(!(*return_value___errno_location$13 == 0))
                                              {
                                                return_value___errno_location$14=__errno_location();
                                                return_value_strerror$15=strerror(*return_value___errno_location$14);
                                                tmp_if_expr$16 = return_value_strerror$15;
                                              }

                                              else
                                                tmp_if_expr$16 = "unknown group";
                                              pt_log(0, "%s: %s\n", argv[(signed long int)i], tmp_if_expr$16);
                                              exit(1);
                                            }

                                            gid = grnam->gr_gid;
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 14:
                                          {
                                            root_dir=strdup(argv[(signed long int)i]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 15:
                                          {
                                            selinux_context=strdup(argv[(signed long int)i]);
                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 16:
                                          {
                                            daemonize = (char)1;
                                            pid_file=fopen(argv[(signed long int)i], "w");
                                            if(pid_file == ((struct _IO_FILE *)NULL))
                                            {
                                              return_value___errno_location$17=__errno_location();
                                              return_value_strerror$18=strerror(*return_value___errno_location$17);
                                              pt_log(0, "%s: %s\n", argv[(signed long int)i], return_value_strerror$18);
                                            }

                                            goto __CPROVER_DUMP_L47;
                                          }
                                          case 0:
                                          {
                                            usage(argv[(signed long int)0]);
                                            return 1;
                                          }
                                          default:
                                          {

                                          __CPROVER_DUMP_L47:
                                            ;
                                            opt = 0;
                                          }
                                        }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if(!(opt == 0))
  {
    usage(argv[(signed long int)0]);
    exit(1);
  }

  if(!(use_udp == 0) && !(pcap == 0))
  {
    pt_log(0, "Packet capture is not supported (or needed) when using UDP for transport.\n");
    pcap = (char)0;
  }

  pt_log(1, "Starting ptunnel v %d.%.2d.\n", 0, 72);
  pt_log(1, "(c) 2004-2011 Daniel Stoedle, <daniels@cs.uit.no>\n");
  pt_log(1, "Security features by Sebastien Raveau, <sebastien.raveau@epita.fr>\n");
  pt_log(1, "%s.\n", mode == 0 ? "Relaying packets from incoming TCP streams" : "Forwarding incoming ping packets over TCP");
  if(!(use_udp == 0))
    pt_log(1, "UDP transport enabled.\n");

  signal(13, (void (*)(signed int))1);
  if(!(use_syslog == 0))
  {
    if(!(log_file == stdout))
    {
      pt_log(0, "Logging using syslog overrides the use of a specified logfile (using -f).\n");
      fclose(log_file);
      log_file = stdout;
    }

    openlog("ptunnel", 0x01, 1 << 3);
  }

  _Bool tmp_if_expr$41;
  signed int return_value_chroot$40;
  if(!(root_dir == ((char *)NULL)))
  {
    pt_log(1, "Restricting file access to %s\n", root_dir);
    signed int return_value_chdir$39;
    return_value_chdir$39=chdir(root_dir);
    if(return_value_chdir$39 == -1)
      tmp_if_expr$41 = (_Bool)1;

    else
    {
      return_value_chroot$40=chroot(root_dir);
      tmp_if_expr$41 = -1 == return_value_chroot$40 ? (_Bool)1 : (_Bool)0;
    }
    if(tmp_if_expr$41)
    {
      signed int *return_value___errno_location$37;
      return_value___errno_location$37=__errno_location();
      char *return_value_strerror$38;
      return_value_strerror$38=strerror(*return_value___errno_location$37);
      pt_log(0, "%s: %s\n", root_dir, return_value_strerror$38);
      exit(1);
    }

  }

  signed int *return_value___errno_location$42;
  char *return_value_strerror$43;
  signed int return_value_setsid$49;
  signed int *return_value___errno_location$44;
  char *return_value_strerror$45;
  signed int *return_value___errno_location$46;
  char *return_value_strerror$47;
  if(!(daemonize == 0))
  {
    pt_log(1, "Going to the background.\n");
    pid=fork();
    if(pid >= 1)
      exit(0);

    if(!(pid >= 0))
    {
      return_value___errno_location$42=__errno_location();
      return_value_strerror$43=strerror(*return_value___errno_location$42);
      pt_log(0, "fork: %s\n", return_value_strerror$43);
    }

    else
    {
      return_value_setsid$49=setsid();
      if(return_value_setsid$49 == -1)
      {
        return_value___errno_location$44=__errno_location();
        return_value_strerror$45=strerror(*return_value___errno_location$44);
        pt_log(0, "setsid: %s\n", return_value_strerror$45);
      }

      else
      {
        pid=fork();
        if(pid >= 1)
          exit(0);

        if(!(pid >= 0))
        {
          return_value___errno_location$46=__errno_location();
          return_value_strerror$47=strerror(*return_value___errno_location$46);
          pt_log(0, "fork: %s\n", return_value_strerror$47);
        }

        else
        {
          if(!(pid_file == ((struct _IO_FILE *)NULL)))
          {
            signed int return_value_getpid$48;
            return_value_getpid$48=getpid();
            fprintf(pid_file, "%d\n", return_value_getpid$48);
            fclose(pid_file);
          }

          freopen("/dev/null", "r", stdin);
          freopen("/dev/null", "w", stdout);
          freopen("/dev/null", "w", stderr);
        }
      }
    }
  }

  pthread_mutex_init(&chain_lock, ((const union anonymous$4 *)NULL));
  pthread_mutex_init(&num_threads_lock, ((const union anonymous$4 *)NULL));
  if(mode == 0)
  {
    if(tcp_listen_port == 0 || tcp_port == 0 || given_dst_ip == 0u || given_proxy_ip == 0u)
    {
      printf("One of the options are missing or invalid.\n");
      usage(argv[(signed long int)0]);
      return -1;
    }

    pt_forwarder();
  }

  else
    pt_proxy(NULL);
  if(!(log_file == stdout))
    fclose(log_file);

  if(!(root_dir == ((char *)NULL)))
    free((void *)root_dir);

  if(!(selinux_context == ((char *)NULL)))
    free((void *)selinux_context);

  pt_log(1, "ptunnel is exiting.\n");
  return 0;
}

// md5_append
// file md5.h line 82
void md5_append(struct md5_state_s *pms, const unsigned char *data, signed int nbytes)
{
  const unsigned char *p = data;
  signed int left = nbytes;
  signed int offset = (signed int)(pms->count[(signed long int)0] >> 3 & (unsigned int)63);
  unsigned int nbits = (unsigned int)(nbytes << 3);
  if(nbytes >= 1)
  {
    pms->count[(signed long int)1] = pms->count[(signed long int)1] + (unsigned int)(nbytes >> 29);
    pms->count[(signed long int)0] = pms->count[(signed long int)0] + nbits;
    if(!(pms->count[0l] >= nbits))
      pms->count[(signed long int)1] = pms->count[(signed long int)1] + 1u;

    if(!(offset == 0))
    {
      signed int copy = offset + nbytes > 64 ? 64 - offset : nbytes;
      memcpy((void *)(pms->buf + (signed long int)offset), (const void *)p, (unsigned long int)copy);
      if(!(copy + offset >= 64))
        goto __CPROVER_DUMP_L7;

      p = p + (signed long int)copy;
      left = left - copy;
      md5_process(pms, pms->buf);
    }

    for( ; left >= 64; left = left - 64)
    {
      md5_process(pms, p);
      p = p + (signed long int)64;
    }
    if(!(left == 0))
      memcpy((void *)pms->buf, (const void *)p, (unsigned long int)left);

  }


__CPROVER_DUMP_L7:
  ;
}

// md5_finish
// file md5.h line 85
void md5_finish(struct md5_state_s *pms, unsigned char *digest)
{
  unsigned char data[8l];
  signed int i = 0;
  for( ; !(i >= 8); i = i + 1)
    data[(signed long int)i] = (unsigned char)(pms->count[(signed long int)(i >> 2)] >> ((i & 3) << 3));
  static const unsigned char pad[64l] = { (const unsigned char)0x80, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0, (const unsigned char)0 };
  md5_append(pms, pad, (signed int)(((unsigned int)55 - (pms->count[(signed long int)0] >> 3) & (unsigned int)63) + (unsigned int)1));
  md5_append(pms, data, 8);
  i = 0;
  for( ; !(i >= 16); i = i + 1)
    digest[(signed long int)i] = (unsigned char)(pms->abcd[(signed long int)(i >> 2)] >> ((i & 3) << 3));
}

// md5_init
// file md5.h line 79
void md5_init(struct md5_state_s *pms)
{
  pms->count[(signed long int)1] = (unsigned int)0;
  pms->count[(signed long int)0] = pms->count[(signed long int)1];
  pms->abcd[(signed long int)0] = (unsigned int)0x67452301;
  pms->abcd[(signed long int)1] = (unsigned int)~0 ^ (unsigned int)0x10325476;
  pms->abcd[(signed long int)2] = (unsigned int)~0 ^ (unsigned int)0x67452301;
  pms->abcd[(signed long int)3] = (unsigned int)0x10325476;
}

// md5_process
// file md5.c line 132
static void md5_process(struct md5_state_s *pms, const unsigned char *data)
{
  unsigned int a = pms->abcd[(signed long int)0];
  unsigned int b = pms->abcd[(signed long int)1];
  unsigned int c = pms->abcd[(signed long int)2];
  unsigned int d = pms->abcd[(signed long int)3];
  unsigned int t;
  unsigned int xbuf[16l];
  const unsigned int *X;
  static const signed int w = 1;
  if(!(*((const unsigned char *)&w) == 0))
  {
    if((data - ((const unsigned char *)NULL) & 3l) == 0l)
      X = (const unsigned int *)data;

    else
    {
      memcpy((void *)xbuf, (const void *)data, (unsigned long int)64);
      X = xbuf;
    }
  }

  else
  {
    const unsigned char *xp = data;
    signed int i;
    X = xbuf;
    i = 0;
    for( ; !(i >= 16); xp = xp + (signed long int)4)
    {
      xbuf[(signed long int)i] = (unsigned int)((signed int)xp[(signed long int)0] + ((signed int)xp[(signed long int)1] << 8) + ((signed int)xp[(signed long int)2] << 16) + ((signed int)xp[(signed long int)3] << 24));
      i = i + 1;
    }
  }
  t = a + (b & c | ~b & d) + X[(signed long int)0] + ((unsigned int)~0 ^ (unsigned int)0x28955b87);
  a = (t << 7 | t >> 32 - 7) + b;
  t = d + (a & b | ~a & c) + X[(signed long int)1] + ((unsigned int)~0 ^ (unsigned int)0x173848a9);
  d = (t << 12 | t >> 32 - 12) + a;
  t = c + (d & a | ~d & b) + X[(signed long int)2] + (unsigned int)0x242070db;
  c = (t << 17 | t >> 32 - 17) + d;
  t = b + (c & d | ~c & a) + X[(signed long int)3] + ((unsigned int)~0 ^ (unsigned int)0x3e423111);
  b = (t << 22 | t >> 32 - 22) + c;
  t = a + (b & c | ~b & d) + X[(signed long int)4] + ((unsigned int)~0 ^ (unsigned int)0x0a83f050);
  a = (t << 7 | t >> 32 - 7) + b;
  t = d + (a & b | ~a & c) + X[(signed long int)5] + (unsigned int)0x4787c62a;
  d = (t << 12 | t >> 32 - 12) + a;
  t = c + (d & a | ~d & b) + X[(signed long int)6] + ((unsigned int)~0 ^ (unsigned int)0x57cfb9ec);
  c = (t << 17 | t >> 32 - 17) + d;
  t = b + (c & d | ~c & a) + X[(signed long int)7] + ((unsigned int)~0 ^ (unsigned int)0x02b96afe);
  b = (t << 22 | t >> 32 - 22) + c;
  t = a + (b & c | ~b & d) + X[(signed long int)8] + (unsigned int)0x698098d8;
  a = (t << 7 | t >> 32 - 7) + b;
  t = d + (a & b | ~a & c) + X[(signed long int)9] + ((unsigned int)~0 ^ (unsigned int)0x74bb0850);
  d = (t << 12 | t >> 32 - 12) + a;
  t = c + (d & a | ~d & b) + X[(signed long int)10] + ((unsigned int)~0 ^ (unsigned int)0x0000a44e);
  c = (t << 17 | t >> 32 - 17) + d;
  t = b + (c & d | ~c & a) + X[(signed long int)11] + ((unsigned int)~0 ^ (unsigned int)0x76a32841);
  b = (t << 22 | t >> 32 - 22) + c;
  t = a + (b & c | ~b & d) + X[(signed long int)12] + (unsigned int)0x6b901122;
  a = (t << 7 | t >> 32 - 7) + b;
  t = d + (a & b | ~a & c) + X[(signed long int)13] + ((unsigned int)~0 ^ (unsigned int)0x02678e6c);
  d = (t << 12 | t >> 32 - 12) + a;
  t = c + (d & a | ~d & b) + X[(signed long int)14] + ((unsigned int)~0 ^ (unsigned int)0x5986bc71);
  c = (t << 17 | t >> 32 - 17) + d;
  t = b + (c & d | ~c & a) + X[(signed long int)15] + (unsigned int)0x49b40821;
  b = (t << 22 | t >> 32 - 22) + c;
  t = a + (b & d | c & ~d) + X[(signed long int)1] + ((unsigned int)~0 ^ (unsigned int)0x09e1da9d);
  a = (t << 5 | t >> 32 - 5) + b;
  t = d + (a & c | b & ~c) + X[(signed long int)6] + ((unsigned int)~0 ^ (unsigned int)0x3fbf4cbf);
  d = (t << 9 | t >> 32 - 9) + a;
  t = c + (d & b | a & ~b) + X[(signed long int)11] + (unsigned int)0x265e5a51;
  c = (t << 14 | t >> 32 - 14) + d;
  t = b + (c & a | d & ~a) + X[(signed long int)0] + ((unsigned int)~0 ^ (unsigned int)0x16493855);
  b = (t << 20 | t >> 32 - 20) + c;
  t = a + (b & d | c & ~d) + X[(signed long int)5] + ((unsigned int)~0 ^ (unsigned int)0x29d0efa2);
  a = (t << 5 | t >> 32 - 5) + b;
  t = d + (a & c | b & ~c) + X[(signed long int)10] + (unsigned int)0x02441453;
  d = (t << 9 | t >> 32 - 9) + a;
  t = c + (d & b | a & ~b) + X[(signed long int)15] + ((unsigned int)~0 ^ (unsigned int)0x275e197e);
  c = (t << 14 | t >> 32 - 14) + d;
  t = b + (c & a | d & ~a) + X[(signed long int)4] + ((unsigned int)~0 ^ (unsigned int)0x182c0437);
  b = (t << 20 | t >> 32 - 20) + c;
  t = a + (b & d | c & ~d) + X[(signed long int)9] + (unsigned int)0x21e1cde6;
  a = (t << 5 | t >> 32 - 5) + b;
  t = d + (a & c | b & ~c) + X[(signed long int)14] + ((unsigned int)~0 ^ (unsigned int)0x3cc8f829);
  d = (t << 9 | t >> 32 - 9) + a;
  t = c + (d & b | a & ~b) + X[(signed long int)3] + ((unsigned int)~0 ^ (unsigned int)0x0b2af278);
  c = (t << 14 | t >> 32 - 14) + d;
  t = b + (c & a | d & ~a) + X[(signed long int)8] + (unsigned int)0x455a14ed;
  b = (t << 20 | t >> 32 - 20) + c;
  t = a + (b & d | c & ~d) + X[(signed long int)13] + ((unsigned int)~0 ^ (unsigned int)0x561c16fa);
  a = (t << 5 | t >> 32 - 5) + b;
  t = d + (a & c | b & ~c) + X[(signed long int)2] + ((unsigned int)~0 ^ (unsigned int)0x03105c07);
  d = (t << 9 | t >> 32 - 9) + a;
  t = c + (d & b | a & ~b) + X[(signed long int)7] + (unsigned int)0x676f02d9;
  c = (t << 14 | t >> 32 - 14) + d;
  t = b + (c & a | d & ~a) + X[(signed long int)12] + ((unsigned int)~0 ^ (unsigned int)0x72d5b375);
  b = (t << 20 | t >> 32 - 20) + c;
  t = a + (b ^ c ^ d) + X[(signed long int)5] + ((unsigned int)~0 ^ (unsigned int)0x0005c6bd);
  a = (t << 4 | t >> 32 - 4) + b;
  t = d + (a ^ b ^ c) + X[(signed long int)8] + ((unsigned int)~0 ^ (unsigned int)0x788e097e);
  d = (t << 11 | t >> 32 - 11) + a;
  t = c + (d ^ a ^ b) + X[(signed long int)11] + (unsigned int)0x6d9d6122;
  c = (t << 16 | t >> 32 - 16) + d;
  t = b + (c ^ d ^ a) + X[(signed long int)14] + ((unsigned int)~0 ^ (unsigned int)0x021ac7f3);
  b = (t << 23 | t >> 32 - 23) + c;
  t = a + (b ^ c ^ d) + X[(signed long int)1] + ((unsigned int)~0 ^ (unsigned int)0x5b4115bb);
  a = (t << 4 | t >> 32 - 4) + b;
  t = d + (a ^ b ^ c) + X[(signed long int)4] + (unsigned int)0x4bdecfa9;
  d = (t << 11 | t >> 32 - 11) + a;
  t = c + (d ^ a ^ b) + X[(signed long int)7] + ((unsigned int)~0 ^ (unsigned int)0x0944b49f);
  c = (t << 16 | t >> 32 - 16) + d;
  t = b + (c ^ d ^ a) + X[(signed long int)10] + ((unsigned int)~0 ^ (unsigned int)0x4140438f);
  b = (t << 23 | t >> 32 - 23) + c;
  t = a + (b ^ c ^ d) + X[(signed long int)13] + (unsigned int)0x289b7ec6;
  a = (t << 4 | t >> 32 - 4) + b;
  t = d + (a ^ b ^ c) + X[(signed long int)0] + ((unsigned int)~0 ^ (unsigned int)0x155ed805);
  d = (t << 11 | t >> 32 - 11) + a;
  t = c + (d ^ a ^ b) + X[(signed long int)3] + ((unsigned int)~0 ^ (unsigned int)0x2b10cf7a);
  c = (t << 16 | t >> 32 - 16) + d;
  t = b + (c ^ d ^ a) + X[(signed long int)6] + (unsigned int)0x04881d05;
  b = (t << 23 | t >> 32 - 23) + c;
  t = a + (b ^ c ^ d) + X[(signed long int)9] + ((unsigned int)~0 ^ (unsigned int)0x262b2fc6);
  a = (t << 4 | t >> 32 - 4) + b;
  t = d + (a ^ b ^ c) + X[(signed long int)12] + ((unsigned int)~0 ^ (unsigned int)0x1924661a);
  d = (t << 11 | t >> 32 - 11) + a;
  t = c + (d ^ a ^ b) + X[(signed long int)15] + (unsigned int)0x1fa27cf8;
  c = (t << 16 | t >> 32 - 16) + d;
  t = b + (c ^ d ^ a) + X[(signed long int)2] + ((unsigned int)~0 ^ (unsigned int)0x3b53a99a);
  b = (t << 23 | t >> 32 - 23) + c;
  t = a + (c ^ (b | ~d)) + X[(signed long int)0] + ((unsigned int)~0 ^ (unsigned int)0x0bd6ddbb);
  a = (t << 6 | t >> 32 - 6) + b;
  t = d + (b ^ (a | ~c)) + X[(signed long int)7] + (unsigned int)0x432aff97;
  d = (t << 10 | t >> 32 - 10) + a;
  t = c + (a ^ (d | ~b)) + X[(signed long int)14] + ((unsigned int)~0 ^ (unsigned int)0x546bdc58);
  c = (t << 15 | t >> 32 - 15) + d;
  t = b + (d ^ (c | ~a)) + X[(signed long int)5] + ((unsigned int)~0 ^ (unsigned int)0x036c5fc6);
  b = (t << 21 | t >> 32 - 21) + c;
  t = a + (c ^ (b | ~d)) + X[(signed long int)12] + (unsigned int)0x655b59c3;
  a = (t << 6 | t >> 32 - 6) + b;
  t = d + (b ^ (a | ~c)) + X[(signed long int)3] + ((unsigned int)~0 ^ (unsigned int)0x70f3336d);
  d = (t << 10 | t >> 32 - 10) + a;
  t = c + (a ^ (d | ~b)) + X[(signed long int)10] + ((unsigned int)~0 ^ (unsigned int)0x00100b82);
  c = (t << 15 | t >> 32 - 15) + d;
  t = b + (d ^ (c | ~a)) + X[(signed long int)1] + ((unsigned int)~0 ^ (unsigned int)0x7a7ba22e);
  b = (t << 21 | t >> 32 - 21) + c;
  t = a + (c ^ (b | ~d)) + X[(signed long int)8] + (unsigned int)0x6fa87e4f;
  a = (t << 6 | t >> 32 - 6) + b;
  t = d + (b ^ (a | ~c)) + X[(signed long int)15] + ((unsigned int)~0 ^ (unsigned int)0x01d3191f);
  d = (t << 10 | t >> 32 - 10) + a;
  t = c + (a ^ (d | ~b)) + X[(signed long int)6] + ((unsigned int)~0 ^ (unsigned int)0x5cfebceb);
  c = (t << 15 | t >> 32 - 15) + d;
  t = b + (d ^ (c | ~a)) + X[(signed long int)13] + (unsigned int)0x4e0811a1;
  b = (t << 21 | t >> 32 - 21) + c;
  t = a + (c ^ (b | ~d)) + X[(signed long int)4] + ((unsigned int)~0 ^ (unsigned int)0x08ac817d);
  a = (t << 6 | t >> 32 - 6) + b;
  t = d + (b ^ (a | ~c)) + X[(signed long int)11] + ((unsigned int)~0 ^ (unsigned int)0x42c50dca);
  d = (t << 10 | t >> 32 - 10) + a;
  t = c + (a ^ (d | ~b)) + X[(signed long int)2] + (unsigned int)0x2ad7d2bb;
  c = (t << 15 | t >> 32 - 15) + d;
  t = b + (d ^ (c | ~a)) + X[(signed long int)9] + ((unsigned int)~0 ^ (unsigned int)0x14792c6e);
  b = (t << 21 | t >> 32 - 21) + c;
  pms->abcd[(signed long int)0] = pms->abcd[(signed long int)0] + a;
  pms->abcd[(signed long int)1] = pms->abcd[(signed long int)1] + b;
  pms->abcd[(signed long int)2] = pms->abcd[(signed long int)2] + c;
  pms->abcd[(signed long int)3] = pms->abcd[(signed long int)3] + d;
}

// pcap_packet_handler
// file ptunnel.c line 892
void pcap_packet_handler(unsigned char *refcon, struct pcap_pkthdr *hdr, const unsigned char *pkt)
{
  struct anonymous$2 *q;
  struct pqueue_elem_t *elem;
  struct anonymous$3 *ip;
  q = (struct anonymous$2 *)refcon;
  void *return_value_malloc$1;
  return_value_malloc$1=malloc((sizeof(struct pqueue_elem_t) /*16ul*/  + (unsigned long int)hdr->caplen) - sizeof(struct ether_header) /*14ul*/ );
  elem = (struct pqueue_elem_t *)return_value_malloc$1;
  memcpy((void *)elem->data, (const void *)(pkt + (signed long int)sizeof(struct ether_header) /*14ul*/ ), (unsigned long int)hdr->caplen - sizeof(struct ether_header) /*14ul*/ );
  ip = (struct anonymous$3 *)elem->data;
  unsigned short int return_value_ntohs$2;
  return_value_ntohs$2=ntohs(ip->pkt_len);
  elem->bytes = (signed int)return_value_ntohs$2;
  if(!((unsigned long int)hdr->caplen + 18446744073709551602ul >= (unsigned long int)elem->bytes))
  {
    pt_log(0, "Received fragmented packet - unable to reconstruct!\n");
    pt_log(0, "This error usually occurs because pcap is used on devices that are not wlan or ethernet.\n");
    free((void *)elem);
  }

  else
  {
    elem->next = ((struct pqueue_elem_t *)NULL);
    if(!(q->tail == ((struct pqueue_elem_t *)NULL)))
    {
      q->tail->next = elem;
      q->tail = elem;
    }

    else
    {
      q->head = elem;
      q->tail = elem;
    }
    q->elems = q->elems + 1;
  }
}

// print_statistics
// file ptunnel.c line 860
void print_statistics(struct xfer_stats_t *xfer, signed int is_continuous)
{
  const double mb = 1024.0 * 1024.0;
  double loss = 0.0;
  if(xfer->icmp_out >= 1u)
    loss = (double)xfer->icmp_resent / (double)xfer->icmp_out;

  if(!(is_continuous == 0))
    printf("\r");

  printf("[inf]: I/O: %6.2f/%6.2f mb ICMP I/O/R: %8d/%8d/%8d Loss: %4.1f%%", xfer->bytes_in / mb, xfer->bytes_out / mb, xfer->icmp_in, xfer->icmp_out, xfer->icmp_resent, loss);
  if(is_continuous == 0)
    printf("\n");

  else
    fflush(stdout);
}

// pt_create_udp_socket
// file ptunnel.c line 542
signed int pt_create_udp_socket(signed int port)
{
  struct sockaddr_in addr;
  signed int sock;
  signed int yes = 1;
  sock=socket(2, 2, 0);
  if(!(sock >= 0))
  {
    pt_log(0, "Failed to set create UDP socket..\n");
    return 0;
  }

  else
  {
    signed int return_value_setsockopt$1;
    return_value_setsockopt$1=setsockopt(sock, 1, 2, (const void *)&yes, (unsigned int)sizeof(signed int) /*4ul*/ );
    if(!(return_value_setsockopt$1 >= 0))
    {
      pt_log(0, "Failed to set UDP REUSEADDR socket option. (Not fatal, hopefully.)\n");
      close(sock);
      return 0;
    }

    else
    {
      yes = 1;
      signed int return_value_setsockopt$2;
      return_value_setsockopt$2=setsockopt(sock, 1, 15, (const void *)&yes, (unsigned int)sizeof(signed int) /*4ul*/ );
      if(!(return_value_setsockopt$2 >= 0))
        pt_log(0, "Failed to set UDP REUSEPORT socket option. (Not fatal, hopefully.)\n");

      memset((void *)&addr, 0, sizeof(struct sockaddr_in) /*16ul*/ );
      addr.sin_family = (unsigned short int)2;
      addr.sin_addr.s_addr=htonl((unsigned int)0x00000000);
      addr.sin_port=htons((unsigned short int)port);
      signed int return_value_bind$3;
      return_value_bind$3=bind(sock, (struct sockaddr *)&addr, (unsigned int)sizeof(struct sockaddr_in) /*16ul*/ );
      if(!(return_value_bind$3 >= 0))
      {
        pt_log(0, "Failed to bind UDP socket to port %d (try running as root).\n", port);
        close(sock);
        return 0;
      }

      else
        return sock;
    }
  }
}

// pt_forwarder
// file ptunnel.c line 467
void pt_forwarder(void)
{
  signed int server_sock;
  signed int new_sock;
  signed int sock;
  signed int yes = 1;
  struct anonymous set;
  struct timeval pt_forwarder$$1$$time;
  struct sockaddr_in addr;
  struct sockaddr_in dest_addr;
  unsigned int addr_len;
  unsigned long int pid;
  unsigned short int rand_id;
  pt_log(4, "Starting forwarder..\n");
  sock=socket(2, 1, 0);
  signed int return_value_setsockopt$3;
  return_value_setsockopt$3=setsockopt(sock, 1, 2, (const void *)&yes, (unsigned int)sizeof(signed int) /*4ul*/ );
  if(return_value_setsockopt$3 == -1)
  {
    signed int *return_value___errno_location$1;
    return_value___errno_location$1=__errno_location();
    char *return_value_strerror$2;
    return_value_strerror$2=strerror(*return_value___errno_location$1);
    pt_log(0, "Failed to set SO_REUSEADDR option on listening socket: %s\n", return_value_strerror$2);
    close(sock);
    goto __CPROVER_DUMP_L13;
  }

  addr.sin_family = (unsigned short int)2;
  addr.sin_port=htons((unsigned short int)tcp_listen_port);
  addr.sin_addr.s_addr = (unsigned int)0x00000000;
  memset((void *)&addr.sin_zero, 0, (unsigned long int)8);
  signed int return_value_bind$6;
  return_value_bind$6=bind(sock, (struct sockaddr *)&addr, (unsigned int)sizeof(struct sockaddr) /*16ul*/ );
  if(return_value_bind$6 == -1)
  {
    signed int *return_value___errno_location$4;
    return_value___errno_location$4=__errno_location();
    char *return_value_strerror$5;
    return_value_strerror$5=strerror(*return_value___errno_location$4);
    pt_log(0, "Failed to bind listening socket: %s\n", return_value_strerror$5);
    close(sock);
    goto __CPROVER_DUMP_L13;
  }

  server_sock = sock;
  memset((void *)&dest_addr, 0, sizeof(struct sockaddr_in) /*16ul*/ );
  dest_addr.sin_family = (unsigned short int)2;
  if(!(use_udp == 0))
    dest_addr.sin_port=htons((unsigned short int)53);

  else
    dest_addr.sin_port = (unsigned short int)0;
  dest_addr.sin_addr.s_addr = given_proxy_ip;
  char *return_value_inet_ntoa$7;
  return_value_inet_ntoa$7=inet_ntoa(*((struct in_addr *)&given_proxy_ip));
  pt_log(3, "Proxy IP address: %s\n", return_value_inet_ntoa$7);
  listen(server_sock, 10);
  while((_Bool)1)
  {
    do
    {
      signed int __d0;
      signed int __d1;
      asm("cld; rep; stosq" : "=c"(__d0), "=D"(__d1) : "a"(0), "0"(sizeof(struct anonymous) /*128ul*/  / sizeof(signed long int) /*8ul*/ ), "1"(&(&set)->__fds_bits[(signed long int)0]) : "memory");
    }
    while((_Bool)0);
    (&set)->__fds_bits[(signed long int)(server_sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] = (&set)->__fds_bits[(signed long int)(server_sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] | (signed long int)(1UL << server_sock % (8 * (signed int)sizeof(signed long int) /*8ul*/ ));
    pt_forwarder$$1$$time.tv_sec = (signed long int)1;
    pt_forwarder$$1$$time.tv_usec = (signed long int)0;
    signed int return_value_select$10;
    return_value_select$10=select(server_sock + 1, &set, ((struct anonymous *)NULL), ((struct anonymous *)NULL), &pt_forwarder$$1$$time);
    if(return_value_select$10 >= 1)
    {
      pt_log(1, "Incoming connection.\n");
      addr_len = (unsigned int)sizeof(struct sockaddr_in) /*16ul*/ ;
      new_sock=accept(server_sock, (struct sockaddr *)&addr, &addr_len);
      if(!(new_sock >= 0))
      {
        pt_log(0, "Accepting incoming connection failed.\n");
        continue;
      }

      pthread_mutex_lock(&num_threads_lock);
      if(!(num_threads >= 1))
      {
        pt_log(2, "No running proxy thread - starting it.\n");
        signed int return_value_pthread_create$8;
        return_value_pthread_create$8=pthread_create(&pid, ((const union pthread_attr_t *)NULL), pt_proxy, NULL);
        if(!(return_value_pthread_create$8 == 0))
        {
          pt_log(0, "Couldn't create thread! Dropping incoming connection.\n");
          close(new_sock);
          pthread_mutex_unlock(&num_threads_lock);
          continue;
        }

      }

      addr = dest_addr;
      signed int return_value_rand$9;
      return_value_rand$9=rand();
      rand_id = (unsigned short int)return_value_rand$9;
      create_and_insert_proxy_desc(rand_id, rand_id, new_sock, &addr, given_dst_ip, (unsigned int)tcp_port, (unsigned int)0, (unsigned int)1073741824);
      pthread_mutex_unlock(&num_threads_lock);
    }

  }

__CPROVER_DUMP_L13:
  ;
}

// pt_log
// file ptunnel.c line 1579
void pt_log(signed int level, char *fmt, ...)
{
  void **args;
  const char *header[6l] = { "[err]: ", "[inf]: ", "[evt]: ", "[vbs]: ", "[dbg]: ", "[xfr]: " };
  signed int syslog_levels[6l] = { 3, 5, 5, 6, 7, 7 };
  if(log_level >= level)
  {
    args = (void **)&fmt;
    if(!(use_syslog == 0))
    {
      char log[255l];
      signed int header_len;
      header_len=snprintf(log, sizeof(char [255l]) /*255ul*/ , "%s", header[(signed long int)level]);
      vsnprintf(log + (signed long int)header_len, sizeof(char [255l]) /*255ul*/  - (unsigned long int)header_len, fmt, args);
      syslog(syslog_levels[(signed long int)level], "%s", (const void *)log);
    }

    else
    {
      fprintf(log_file, "%s", header[(signed long int)level]);
      vfprintf(log_file, fmt, args);
    }
    args = ((void **)NULL);
    if(use_syslog == 0 && !(log_file == stdout))
      fflush(log_file);

  }

}

// pt_proxy
// file ptunnel.c line 578
void * pt_proxy(void *args)
{
  struct anonymous set;
  struct timeval timeout;
  signed int bytes;
  struct sockaddr_in addr;
  unsigned int addr_len;
  signed int fwd_sock = 0;
  signed int max_sock = 0;
  signed int idx;
  char *buf;
  double now;
  double last_status_update = 0.0;
  struct proxy_desc_t *pt_proxy$$1$$cur;
  struct proxy_desc_t *prev;
  struct proxy_desc_t *tmp;
  struct anonymous$5 pc;
  struct xfer_stats_t xfer;
  pt_log(4, "Starting ping proxy..\n");
  if(!(use_udp == 0))
  {
    pt_log(4, "Creating UDP socket..\n");
    if(mode == 1)
      fwd_sock=pt_create_udp_socket(53);

    else
      fwd_sock=pt_create_udp_socket(0);
    if(fwd_sock == 0)
    {
      pt_log(0, "Failed to create UDP socket.\n");
      return NULL;
    }

  }

  else
  {
    if(!(unprivileged == 0))
    {
      pt_log(4, "Attempting to create unprivileged ICMP datagram socket..\n");
      fwd_sock=socket(2, 2, 1);
    }

    else
    {
      pt_log(4, "Attempting to create privileged ICMP raw socket..\n");
      fwd_sock=socket(2, 3, 1);
    }
    if(!(fwd_sock >= 0))
    {
      signed int *return_value___errno_location$1;
      return_value___errno_location$1=__errno_location();
      char *return_value_strerror$2;
      return_value_strerror$2=strerror(*return_value___errno_location$1);
      pt_log(0, "Couldn't create %s socket: %s\n", unprivileged != 0 ? "unprivileged datagram" : "privileged raw", return_value_strerror$2);
      return NULL;
    }

  }
  max_sock = fwd_sock + 1;
  signed int return_value_pcap_setfilter$8;
  if(!(pcap == 0))
  {
    if(!(use_udp == 0))
    {
      pt_log(0, "Packet capture is not useful with UDP [should not get here!]!\n");
      close(fwd_sock);
      return NULL;
    }

    if(unprivileged == 0)
    {
      pt_log(1, "Initializing pcap.\n");
      void *return_value_malloc$3;
      return_value_malloc$3=malloc((unsigned long int)256);
      pc.pcap_err_buf = (char *)return_value_malloc$3;
      void *return_value_malloc$4;
      return_value_malloc$4=malloc((unsigned long int)pcap_buf_size);
      pc.pcap_data_buf = (char *)return_value_malloc$4;
      pc.pcap_desc=pcap_open_live(pcap_device, pcap_buf_size, 0, 50, pc.pcap_err_buf);
      if(!(pc.pcap_desc == ((struct pcap *)NULL)))
      {
        signed int return_value_pcap_lookupnet$5;
        return_value_pcap_lookupnet$5=pcap_lookupnet(pcap_device, &pc.netp, &pc.netmask, pc.pcap_err_buf);
        if(return_value_pcap_lookupnet$5 == -1)
        {
          pt_log(0, "pcap error: %s\n", pc.pcap_err_buf);
          pcap = (char)0;
        }

        char *return_value_inet_ntoa$6;
        return_value_inet_ntoa$6=inet_ntoa(*((struct in_addr *)&pc.netp));
        pt_log(3, "Network: %s\n", return_value_inet_ntoa$6);
        char *return_value_inet_ntoa$7;
        return_value_inet_ntoa$7=inet_ntoa(*((struct in_addr *)&pc.netmask));
        pt_log(3, "Netmask: %s\n", return_value_inet_ntoa$7);
        signed int return_value_pcap_compile$9;
        return_value_pcap_compile$9=pcap_compile(pc.pcap_desc, &pc.fp, pcap_filter_program, 0, pc.netp);
        if(return_value_pcap_compile$9 == -1)
        {
          pt_log(0, "Failed to compile pcap filter program.\n");
          pcap_close(pc.pcap_desc);
          pcap = (char)0;
        }

        else
        {
          return_value_pcap_setfilter$8=pcap_setfilter(pc.pcap_desc, &pc.fp);
          if(return_value_pcap_setfilter$8 == -1)
          {
            pt_log(0, "Failed to set pcap filter program.\n");
            pcap_close(pc.pcap_desc);
            pcap = (char)0;
          }

        }
      }

      else
      {
        pt_log(0, "pcap error: %s\n", pc.pcap_err_buf);
        pcap = (char)0;
      }
      pc.pkt_q.head = ((struct pqueue_elem_t *)NULL);
      pc.pkt_q.tail = ((struct pqueue_elem_t *)NULL);
      pc.pkt_q.elems = 0;
      if(pcap == 0)
      {
        pt_log(0, "There were errors enabling pcap - pcap has been disabled.\n");
        free((void *)pc.pcap_err_buf);
        free((void *)pc.pcap_data_buf);
        return NULL;
      }

    }

    else
      pt_log(1, "pcap disabled since we're running in unprivileged mode.\n");
  }

  pthread_mutex_lock(&num_threads_lock);
  num_threads = num_threads + 1;
  pthread_mutex_unlock(&num_threads_lock);
  void *return_value_malloc$10;
  return_value_malloc$10=malloc((unsigned long int)icmp_receive_buf_len);
  buf = (char *)return_value_malloc$10;
  pt_log(1, "Ping proxy is listening in %s mode.\n", unprivileged != 0 ? "unprivileged" : "privileged");
  if(!(selinux_context == ((char *)NULL)) || !(gid == 0u) || !(uid == 0u))
    pt_log(1, "Dropping privileges now.\n");

  signed int return_value_setgid$13;
  signed int *return_value___errno_location$11;
  char *return_value_strerror$12;
  if(!(gid == 0u))
  {
    return_value_setgid$13=setgid(gid);
    if(return_value_setgid$13 == -1)
    {
      return_value___errno_location$11=__errno_location();
      return_value_strerror$12=strerror(*return_value___errno_location$11);
      pt_log(0, "setgid(%d): %s\n", gid, return_value_strerror$12);
    }

  }

  signed int return_value_setuid$16;
  signed int *return_value___errno_location$14;
  char *return_value_strerror$15;
  if(!(uid == 0u))
  {
    return_value_setuid$16=setuid(uid);
    if(return_value_setuid$16 == -1)
    {
      return_value___errno_location$14=__errno_location();
      return_value_strerror$15=strerror(*return_value___errno_location$14);
      pt_log(0, "setuid(%d): %s\n", uid, return_value_strerror$15);
    }

  }

  signed int return_value_setcon$19;
  signed int *return_value___errno_location$17;
  char *return_value_strerror$18;
  if(!(selinux_context == ((char *)NULL)))
  {
    return_value_setcon$19=setcon(selinux_context);
    if(return_value_setcon$19 == -1)
    {
      return_value___errno_location$17=__errno_location();
      return_value_strerror$18=strerror(*return_value___errno_location$17);
      pt_log(0, "setcon(%s) failed: %s\n", selinux_context, return_value_strerror$18);
    }

  }

  _Bool tmp_if_expr$20;
  _Bool tmp_if_expr$22;
  unsigned int return_value_send_packets$27;
  while((_Bool)1)
  {
    do
    {
      signed int __d0;
      signed int __d1;
      asm("cld; rep; stosq" : "=c"(__d0), "=D"(__d1) : "a"(0), "0"(sizeof(struct anonymous) /*128ul*/  / sizeof(signed long int) /*8ul*/ ), "1"(&(&set)->__fds_bits[(signed long int)0]) : "memory");
    }
    while((_Bool)0);
    (&set)->__fds_bits[(signed long int)(fwd_sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] = (&set)->__fds_bits[(signed long int)(fwd_sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] | (signed long int)(1UL << fwd_sock % (8 * (signed int)sizeof(signed long int) /*8ul*/ ));
    max_sock = fwd_sock + 1;
    pthread_mutex_lock(&chain_lock);
    pt_proxy$$1$$cur = chain;
    for( ; !(pt_proxy$$1$$cur == ((struct proxy_desc_t *)NULL)); pt_proxy$$1$$cur = pt_proxy$$1$$cur->next)
      if(!(pt_proxy$$1$$cur->sock == 0))
      {
        (&set)->__fds_bits[(signed long int)(pt_proxy$$1$$cur->sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] = (&set)->__fds_bits[(signed long int)(pt_proxy$$1$$cur->sock / (8 * (signed int)sizeof(signed long int) /*8ul*/ ))] | (signed long int)(1UL << pt_proxy$$1$$cur->sock % (8 * (signed int)sizeof(signed long int) /*8ul*/ ));
        if(pt_proxy$$1$$cur->sock >= max_sock)
          max_sock = pt_proxy$$1$$cur->sock + 1;

      }

    pthread_mutex_unlock(&chain_lock);
    timeout.tv_sec = (signed long int)0;
    timeout.tv_usec = (signed long int)10000;
    select(max_sock, &set, ((struct anonymous *)NULL), ((struct anonymous *)NULL), &timeout);
    pthread_mutex_lock(&chain_lock);
    prev = ((struct proxy_desc_t *)NULL);
    pt_proxy$$1$$cur = chain;
    do
    {
      if(!(pt_proxy$$1$$cur == ((struct proxy_desc_t *)NULL)))
        tmp_if_expr$20 = pt_proxy$$1$$cur->sock != 0 ? (_Bool)1 : (_Bool)0;

      else
        tmp_if_expr$20 = (_Bool)0;
      if(!tmp_if_expr$20)
        break;

      if(pt_proxy$$1$$cur->state == 0u)
      {
        pt_log(3, "Sending proxy request.\n");
        pt_proxy$$1$$cur->last_ack=time_as_double();
        queue_packet(fwd_sock, (unsigned char)pt_proxy$$1$$cur->pkt_type, ((char *)NULL), 0, pt_proxy$$1$$cur->id_no, pt_proxy$$1$$cur->id_no, &pt_proxy$$1$$cur->my_seq, pt_proxy$$1$$cur->send_ring, &pt_proxy$$1$$cur->send_idx, &pt_proxy$$1$$cur->send_wait_ack, pt_proxy$$1$$cur->dst_ip, pt_proxy$$1$$cur->dst_port, pt_proxy$$1$$cur->state | pt_proxy$$1$$cur->type_flag, &pt_proxy$$1$$cur->dest_addr, pt_proxy$$1$$cur->next_remote_seq, &pt_proxy$$1$$cur->send_first_ack, &pt_proxy$$1$$cur->ping_seq);
        pt_proxy$$1$$cur->xfer.icmp_out = pt_proxy$$1$$cur->xfer.icmp_out + 1u;
        pt_proxy$$1$$cur->state = (unsigned int)2;
      }

      if(!(pt_proxy$$1$$cur->should_remove == 0))
      {
        pt_log(1, "\nSession statistics:\n");
        print_statistics(&pt_proxy$$1$$cur->xfer, 0);
        pt_log(1, "\n");
        tmp = pt_proxy$$1$$cur->next;
        remove_proxy_desc(pt_proxy$$1$$cur, prev);
      }

      else
      {
        if(!((set.__fds_bits[(signed long int)(pt_proxy$$1$$cur->sock / 8)] & (signed long int)(1UL << pt_proxy$$1$$cur->sock % 8)) == 0l))
        {
          if(!(pt_proxy$$1$$cur->send_wait_ack >= 64))
          {
            if(password == ((char *)NULL))
              tmp_if_expr$22 = (_Bool)1;

            else
              tmp_if_expr$22 = pt_proxy$$1$$cur->authenticated != 0 ? (_Bool)1 : (_Bool)0;
            if(tmp_if_expr$22)
            {
              signed long int return_value_recv$21;
              return_value_recv$21=recv(pt_proxy$$1$$cur->sock, (void *)pt_proxy$$1$$cur->buf, (unsigned long int)tcp_receive_buf_len, 0);
              bytes = (signed int)return_value_recv$21;
              if(!(bytes >= 1))
              {
                pt_log(1, "Connection closed or lost.\n");
                tmp = pt_proxy$$1$$cur->next;
                send_termination_msg(pt_proxy$$1$$cur, fwd_sock);
                pt_log(1, "Session statistics:\n");
                print_statistics(&pt_proxy$$1$$cur->xfer, 0);
                remove_proxy_desc(pt_proxy$$1$$cur, prev);
                goto __CPROVER_DUMP_L34;
              }

              pt_proxy$$1$$cur->xfer.bytes_out = pt_proxy$$1$$cur->xfer.bytes_out + (double)bytes;
              pt_proxy$$1$$cur->xfer.icmp_out = pt_proxy$$1$$cur->xfer.icmp_out + 1u;
              queue_packet(fwd_sock, (unsigned char)pt_proxy$$1$$cur->pkt_type, pt_proxy$$1$$cur->buf, bytes, pt_proxy$$1$$cur->id_no, pt_proxy$$1$$cur->icmp_id, &pt_proxy$$1$$cur->my_seq, pt_proxy$$1$$cur->send_ring, &pt_proxy$$1$$cur->send_idx, &pt_proxy$$1$$cur->send_wait_ack, (unsigned int)0, (unsigned int)0, pt_proxy$$1$$cur->state | pt_proxy$$1$$cur->type_flag, &pt_proxy$$1$$cur->dest_addr, pt_proxy$$1$$cur->next_remote_seq, &pt_proxy$$1$$cur->send_first_ack, &pt_proxy$$1$$cur->ping_seq);
            }

          }

        }

        prev = pt_proxy$$1$$cur;
        tmp = pt_proxy$$1$$cur->next;
      }

    __CPROVER_DUMP_L34:
      ;
      pt_proxy$$1$$cur = tmp;
    }
    while((_Bool)1);
    pthread_mutex_unlock(&chain_lock);
    if(!((set.__fds_bits[(signed long int)(fwd_sock / 8)] & (signed long int)(1UL << fwd_sock % 8)) == 0l))
    {
      addr_len = (unsigned int)sizeof(struct sockaddr) /*16ul*/ ;
      signed long int return_value_recvfrom$23;
      return_value_recvfrom$23=recvfrom(fwd_sock, (void *)buf, (unsigned long int)icmp_receive_buf_len, 0, (struct sockaddr *)&addr, &addr_len);
      bytes = (signed int)return_value_recvfrom$23;
      if(!(bytes >= 0))
      {
        signed int *return_value___errno_location$24;
        return_value___errno_location$24=__errno_location();
        char *return_value_strerror$25;
        return_value_strerror$25=strerror(*return_value___errno_location$24);
        pt_log(0, "Error receiving packet on ICMP socket: %s\n", return_value_strerror$25);
        break;
      }

      handle_packet(buf, bytes, 0, &addr, fwd_sock);
    }

    pthread_mutex_lock(&chain_lock);
    now=time_as_double();
    pt_proxy$$1$$cur = chain;
    for( ; !(pt_proxy$$1$$cur == ((struct proxy_desc_t *)NULL)); pt_proxy$$1$$cur = pt_proxy$$1$$cur->next)
    {
      if(pt_proxy$$1$$cur->last_activity + 60.000000 < now)
      {
        char *return_value_inet_ntoa$26;
        return_value_inet_ntoa$26=inet_ntoa(*((struct in_addr *)&pt_proxy$$1$$cur->dst_ip));
        pt_log(1, "Dropping tunnel to %s:%d due to inactivity.\n", return_value_inet_ntoa$26, pt_proxy$$1$$cur->dst_port, pt_proxy$$1$$cur->id_no);
        pt_proxy$$1$$cur->should_remove = 1;
        goto __CPROVER_DUMP_L43;
      }

      if(!(pt_proxy$$1$$cur->recv_wait_send == 0))
      {
        if(!(pt_proxy$$1$$cur->sock == 0))
        {
          return_value_send_packets$27=send_packets(pt_proxy$$1$$cur->recv_ring, &pt_proxy$$1$$cur->recv_xfer_idx, &pt_proxy$$1$$cur->recv_wait_send, &pt_proxy$$1$$cur->sock);
          pt_proxy$$1$$cur->xfer.bytes_in = pt_proxy$$1$$cur->xfer.bytes_in + (double)return_value_send_packets$27;
        }

      }

      idx = pt_proxy$$1$$cur->send_first_ack;
      if(!(pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt == ((struct anonymous$6 *)NULL)))
      {
        if(pt_proxy$$1$$cur->send_ring[(signed long int)idx].last_resend + 1.5 < now)
        {
          pt_log(4, "Resending packet with seq-no %d.\n", pt_proxy$$1$$cur->send_ring[(signed long int)idx].seq_no);
          pt_proxy$$1$$cur->send_ring[(signed long int)idx].last_resend = now;
          pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt->seq=htons(pt_proxy$$1$$cur->ping_seq);
          pt_proxy$$1$$cur->ping_seq = pt_proxy$$1$$cur->ping_seq + 1;
          pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt->checksum = (unsigned short int)0;
          unsigned short int return_value_calc_icmp_checksum$28;
          return_value_calc_icmp_checksum$28=calc_icmp_checksum((unsigned short int *)pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt, pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt_len);
          pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt->checksum=htons(return_value_calc_icmp_checksum$28);
          sendto(fwd_sock, (const void *)pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt, (unsigned long int)pt_proxy$$1$$cur->send_ring[(signed long int)idx].pkt_len, 0, (struct sockaddr *)&pt_proxy$$1$$cur->dest_addr, (unsigned int)sizeof(struct sockaddr) /*16ul*/ );
          pt_proxy$$1$$cur->xfer.icmp_resent = pt_proxy$$1$$cur->xfer.icmp_resent + 1u;
        }

      }

      if(pt_proxy$$1$$cur->last_ack + 1.0 < now)
      {
        if(!(pt_proxy$$1$$cur->send_wait_ack >= 64))
        {
          if(!(1 + (signed int)pt_proxy$$1$$cur->remote_ack_val == (signed int)pt_proxy$$1$$cur->next_remote_seq))
          {
            pt_proxy$$1$$cur->last_ack = now;
            queue_packet(fwd_sock, (unsigned char)pt_proxy$$1$$cur->pkt_type, ((char *)NULL), 0, pt_proxy$$1$$cur->id_no, pt_proxy$$1$$cur->icmp_id, &pt_proxy$$1$$cur->my_seq, pt_proxy$$1$$cur->send_ring, &pt_proxy$$1$$cur->send_idx, &pt_proxy$$1$$cur->send_wait_ack, pt_proxy$$1$$cur->dst_ip, pt_proxy$$1$$cur->dst_port, (unsigned int)1 | pt_proxy$$1$$cur->type_flag, &pt_proxy$$1$$cur->dest_addr, pt_proxy$$1$$cur->next_remote_seq, &pt_proxy$$1$$cur->send_first_ack, &pt_proxy$$1$$cur->ping_seq);
            pt_proxy$$1$$cur->xfer.icmp_ack_out = pt_proxy$$1$$cur->xfer.icmp_ack_out + 1u;
          }

        }

      }


    __CPROVER_DUMP_L43:
      ;
    }
    pthread_mutex_unlock(&chain_lock);
    if(!(pcap == 0))
    {
      signed int return_value_pcap_dispatch$29;
      return_value_pcap_dispatch$29=pcap_dispatch(pc.pcap_desc, 32, pcap_packet_handler, (unsigned char *)&pc.pkt_q);
      if(return_value_pcap_dispatch$29 >= 1)
      {
        struct pqueue_elem_t *cur;
        for( ; !(pc.pkt_q.head == ((struct pqueue_elem_t *)NULL)); pc.pkt_q.elems = pc.pkt_q.elems - 1)
        {
          cur = pc.pkt_q.head;
          memset((void *)&addr, (signed int)sizeof(struct sockaddr) /*16ul*/ , (unsigned long int)0);
          addr.sin_family = (unsigned short int)2;
          addr.sin_addr.s_addr = *((unsigned int *)&((struct anonymous$3 *)cur->data)->src_ip);
          handle_packet(cur->data, cur->bytes, 1, &addr, fwd_sock);
          pc.pkt_q.head = cur->next;
          free((void *)cur);
        }
        pc.pkt_q.tail = ((struct pqueue_elem_t *)NULL);
        pc.pkt_q.head = ((struct pqueue_elem_t *)NULL);
      }

    }

    if(mode == 0 && now > last_status_update + 1.000000 && !(print_stats == 0))
    {
      pthread_mutex_lock(&chain_lock);
      memset((void *)&xfer, 0, sizeof(struct xfer_stats_t) /*32ul*/ );
      pt_proxy$$1$$cur = chain;
      for( ; !(pt_proxy$$1$$cur == ((struct proxy_desc_t *)NULL)); pt_proxy$$1$$cur = pt_proxy$$1$$cur->next)
      {
        xfer.bytes_in = xfer.bytes_in + pt_proxy$$1$$cur->xfer.bytes_in;
        xfer.bytes_out = xfer.bytes_out + pt_proxy$$1$$cur->xfer.bytes_out;
        xfer.icmp_in = xfer.icmp_in + pt_proxy$$1$$cur->xfer.icmp_in;
        xfer.icmp_out = xfer.icmp_out + pt_proxy$$1$$cur->xfer.icmp_out;
        xfer.icmp_resent = xfer.icmp_resent + pt_proxy$$1$$cur->xfer.icmp_resent;
      }
      pthread_mutex_unlock(&chain_lock);
      print_statistics(&xfer, 1);
      last_status_update = now;
    }

  }
  pt_log(4, "Proxy exiting..\n");
  if(!(fwd_sock == 0))
    close(fwd_sock);

  free((void *)buf);
  pt_log(4, "Ping proxy done\n");
  return NULL;
}

// queue_packet
// file ptunnel.c line 1205
signed int queue_packet(signed int icmp_sock, unsigned char type, char *buf, signed int num_bytes, unsigned short int id_no, unsigned short int icmp_id, unsigned short int *seq, struct anonymous$8 *ring, signed int *insert_idx, signed int *await_send, unsigned int ip, unsigned int port, unsigned int state, struct sockaddr_in *dest_addr, unsigned short int next_expected_seq, signed int *first_ack, unsigned short int *ping_seq)
{
  signed int pkt_len = (signed int)(sizeof(struct anonymous$6) /*8ul*/  + sizeof(struct anonymous$1) /*28ul*/  + (unsigned long int)num_bytes);
  signed int err = 0;
  struct anonymous$6 *pkt = ((struct anonymous$6 *)NULL);
  struct anonymous$1 *pt_pkt = ((struct anonymous$1 *)NULL);
  unsigned short int ack_val = (unsigned short int)((signed int)next_expected_seq - 1);
  if(!(pkt_len % 2 == 0))
    pkt_len = pkt_len + 1;

  void *return_value_malloc$1;
  return_value_malloc$1=malloc((unsigned long int)pkt_len);
  pkt = (struct anonymous$6 *)return_value_malloc$1;
  pkt->type = type;
  pkt->code = (unsigned char)0;
  pkt->identifier=htons(icmp_id);
  pkt->seq=htons(*ping_seq);
  pkt->checksum = (unsigned short int)0;
  *ping_seq = *ping_seq + 1;
  pt_pkt = (struct anonymous$1 *)pkt->data;
  pt_pkt->magic=htonl(0xD5200880);
  pt_pkt->dst_ip = ip;
  pt_pkt->dst_port=htonl(port);
  pt_pkt->ack=htonl((unsigned int)ack_val);
  pt_pkt->data_len=htonl((unsigned int)num_bytes);
  pt_pkt->state=htonl(state);
  pt_pkt->seq_no=htons(*seq);
  pt_pkt->id_no=htons(id_no);
  if(num_bytes >= 1 && !(buf == ((char *)NULL)))
    memcpy((void *)pt_pkt->data, (const void *)buf, (unsigned long int)num_bytes);

  unsigned short int return_value_calc_icmp_checksum$2;
  return_value_calc_icmp_checksum$2=calc_icmp_checksum((unsigned short int *)pkt, pkt_len);
  pkt->checksum=htons(return_value_calc_icmp_checksum$2);
  pt_log(5, "Send: %d [%d] bytes [seq = %d] [type = %s] [ack = %d] [icmp = %d] [user = %s]\n", pkt_len, num_bytes, *seq, state_name[(signed long int)(state & (unsigned int)~-1073741824)], ack_val, type, (state & (unsigned int)1073741824) == (unsigned int)1073741824 ? "yes" : "no");
  signed long int return_value_sendto$3;
  return_value_sendto$3=sendto(icmp_sock, (const void *)pkt, (unsigned long int)pkt_len, 0, (struct sockaddr *)dest_addr, (unsigned int)sizeof(struct sockaddr) /*16ul*/ );
  err = (signed int)return_value_sendto$3;
  if(!(err >= 0))
  {
    signed int *return_value___errno_location$4;
    return_value___errno_location$4=__errno_location();
    char *return_value_strerror$5;
    return_value_strerror$5=strerror(*return_value___errno_location$4);
    pt_log(0, "Failed to send ICMP packet: %s\n", return_value_strerror$5);
    return -1;
  }

  else
    if(!(err == pkt_len))
      pt_log(0, "WARNING WARNING, didn't send entire packet\n");

  (ring + (signed long int)*insert_idx)->pkt = pkt;
  (ring + (signed long int)*insert_idx)->pkt_len = pkt_len;
  (ring + (signed long int)*insert_idx)->last_resend=time_as_double();
  (ring + (signed long int)*insert_idx)->seq_no = *seq;
  (ring + (signed long int)*insert_idx)->icmp_id = icmp_id;
  *seq = *seq + 1;
  if((ring + (signed long int)*first_ack)->pkt == ((struct anonymous$6 *)NULL))
    *first_ack = *insert_idx;

  *await_send = *await_send + 1;
  *insert_idx = *insert_idx + 1;
  if(*insert_idx >= 64)
    *insert_idx = 0;

  return 0;
}

// remove_proxy_desc
// file ptunnel.c line 1162
void remove_proxy_desc(struct proxy_desc_t *cur, struct proxy_desc_t *prev)
{
  signed int i;
  struct timeval tt;
  pt_log(4, "Removing proxy descriptor.\n");
  gettimeofday(&tt, ((struct timezone *)NULL));
  seq_expiry_tbl[(signed long int)cur->id_no] = (unsigned int)(tt.tv_sec + (signed long int)(2 * 60));
  if(!(cur->buf == ((char *)NULL)))
    free((void *)cur->buf);

  cur->buf = ((char *)NULL);
  i = 0;
  for( ; !(i >= 64); i = i + 1)
  {
    if(!(cur->send_ring[(signed long int)i].pkt == ((struct anonymous$6 *)NULL)))
      free((void *)cur->send_ring[(signed long int)i].pkt);

    cur->send_ring[(signed long int)i].pkt = ((struct anonymous$6 *)NULL);
    if(!(cur->recv_ring[(signed long int)i] == ((struct anonymous$7 *)NULL)))
      free((void *)cur->recv_ring[(signed long int)i]);

    cur->recv_ring[(signed long int)i] = ((struct anonymous$7 *)NULL);
  }
  close(cur->sock);
  cur->sock = 0;
  if(!(prev == ((struct proxy_desc_t *)NULL)))
    prev->next = cur->next;

  else
    chain = cur->next;
  if(!(cur->challenge == ((struct challenge_t *)NULL)))
    free((void *)cur->challenge);

  free((void *)cur);
  num_tunnels = num_tunnels - 1;
}

// send_packets
// file ptunnel.c line 1306
unsigned int send_packets(struct anonymous$7 **ring, signed int *xfer_idx, signed int *await_send, signed int *sock)
{
  struct anonymous$7 *fwd_desc;
  signed int bytes;
  signed int total = 0;
  while(*await_send >= 1)
  {
    fwd_desc = ring[(signed long int)*xfer_idx];
    if(fwd_desc == ((struct anonymous$7 *)NULL))
      break;

    if(fwd_desc->length >= 1)
    {
      signed long int return_value_send$1;
      return_value_send$1=send(*sock, (const void *)&fwd_desc->data[(signed long int)(fwd_desc->length - fwd_desc->remaining)], (unsigned long int)fwd_desc->remaining, 0);
      bytes = (signed int)return_value_send$1;
      if(!(bytes >= 0))
      {
        printf("Weirdness.\n");
        close(*sock);
        *sock = 0;
        break;
      }

      fwd_desc->remaining = fwd_desc->remaining - bytes;
      total = total + bytes;
    }

    if(fwd_desc->remaining == 0)
    {
      ring[(signed long int)*xfer_idx] = ((struct anonymous$7 *)NULL);
      free((void *)fwd_desc);
      *xfer_idx = *xfer_idx + 1;
      *await_send = *await_send - 1;
      if(*xfer_idx >= 64)
        *xfer_idx = 0;

    }

    else
      break;
  }
  return (unsigned int)total;
}

// send_termination_msg
// file ptunnel.c line 1571
void send_termination_msg(struct proxy_desc_t *cur, signed int icmp_sock)
{
  queue_packet(icmp_sock, (unsigned char)cur->pkt_type, ((char *)NULL), 0, cur->id_no, cur->icmp_id, &cur->my_seq, cur->send_ring, &cur->send_idx, &cur->send_wait_ack, (unsigned int)0, (unsigned int)0, (unsigned int)3 | cur->type_flag, &cur->dest_addr, cur->next_remote_seq, &cur->send_first_ack, &cur->ping_seq);
  queue_packet(icmp_sock, (unsigned char)cur->pkt_type, ((char *)NULL), 0, cur->id_no, cur->icmp_id, &cur->my_seq, cur->send_ring, &cur->send_idx, &cur->send_wait_ack, (unsigned int)0, (unsigned int)0, (unsigned int)3 | cur->type_flag, &cur->dest_addr, cur->next_remote_seq, &cur->send_first_ack, &cur->ping_seq);
  cur->xfer.icmp_out = cur->xfer.icmp_out + (unsigned int)2;
}

// time_as_double
// file ptunnel.c line 1615
double time_as_double(void)
{
  double result;
  struct timeval tt;
  gettimeofday(&tt, ((struct timezone *)NULL));
  result = (double)tt.tv_sec + (double)tt.tv_usec / (double)10e5;
  return result;
}

// usage
// file ptunnel.c line 415
void usage(char *exec_name)
{
  printf("ptunnel v %d.%.2d.\n", 0, 72);
  printf("Usage:   %s -p <addr> -lp <port> -da <dest_addr> -dp <dest_port> [-m max_tunnels] [-v verbosity] [-f logfile]\n", exec_name);
  printf("         %s [-m max_threads] [-v verbosity] [-c <device>]\n", exec_name);
  printf("     -p: Set address of peer running packet forwarder. This causes\n");
  printf("         ptunnel to operate in forwarding mode - the absence of this\n");
  printf("         option causes ptunnel to operate in proxy mode.\n");
  printf("    -lp: Set TCP listening port (only used when operating in forward mode)\n");
  printf("    -da: Set remote proxy destination address if client\n");
  printf("         Restrict to only this destination address if server\n");
  printf("    -dp: Set remote proxy destionation port if client\n");
  printf("         Restrict to only this destination port if server\n");
  printf("     -m: Set maximum number of concurrent tunnels\n");
  printf("     -v: Verbosity level (-1 to 4, where -1 is no output, and 4 is all output)\n");
  printf("     -c: Enable libpcap on the given device.\n");
  printf("     -f: Specify a file to log to, rather than printing to standard out.\n");
  printf("     -s: Client only. Enables continuous output of statistics (packet loss, etc.)\n");
  printf("-daemon: Run in background, the PID will be written in the file supplied as argument\n");
  printf("-syslog: Output debug to syslog instead of standard out.\n");
  printf("   -udp: Toggle use of UDP instead of ICMP. Proxy will listen on port 53 (must be root).\n\n");
  printf("Security features:  [-x password] [-u] [-setuid user] [-setgid group] [-chroot dir]\n");
  printf("     -x: Set password (must be same on client and proxy)\n");
  printf("     -u: Run proxy in unprivileged mode. This causes the proxy to forward\n");
  printf("         packets using standard echo requests, instead of crafting custom echo replies.\n");
  printf("         Unprivileged mode will only work on some systems, and is in general less reliable\n");
  printf("         than running in privileged mode.\n");
  printf("         Please consider combining the following three options instead:\n");
  printf("-setuid: When started in privileged mode, drop down to user's rights as soon as possible\n");
  printf("-setgid: When started in privileged mode, drop down to group's rights as soon as possible\n");
  printf("-chroot: When started in privileged mode, restrict file access to the specified directory\n");
  printf("-setcon: Set SELinux context when all there is left to do are network I/O operations\n");
  printf("         To combine with -chroot you will have to `mount --bind /proc /chrootdir/proc`\n");
  printf("\nStarting the proxy (needs to run as root):\n");
  printf(" [root #] %s\n", exec_name);
  printf("Starting a client (also needs root):\n");
  printf(" [root #] %s -p proxy.pingtunnel.com -lp 8000 -da login.domain.com -dp 22 -c eth0\n", exec_name);
  printf("And then using the tunnel to ssh to login.domain.com:\n");
  printf(" [user $] ssh -p 8000 localhost\n");
  printf("And that's it. Enjoy your tunnel!\n\n");
}

// validate_challenge
// file ptunnel.c line 1560
signed int validate_challenge(struct challenge_t *local, struct challenge_t *remote)
{
  generate_response(local);
  signed int return_value_memcmp$1;
  return_value_memcmp$1=memcmp((const void *)local, (const void *)remote, sizeof(struct challenge_t) /*32ul*/ );
  if(return_value_memcmp$1 == 0)
    return 1;

  else
    return 0;
}

