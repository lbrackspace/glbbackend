#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/select.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>


#define STRSIZE 1024
#define BLOCKSIZE 4096
#define INITRINGSIZE 512

typedef struct {
    int h;
    int size;
    int used;
    char *data;
}ringcharbuff_t;



int usage(char *prog);
ssize_t myread(int fd,char *block,size_t blocksize,int *eof);
ssize_t mywrite(int fd,char *block,size_t blocksize,int *eof);
int setnonblocking(int s);
int lookup(char *host,char *port,int ai_family,struct addrinfo **result);
int echo_client(int cs);
int main(int argc, char **argv);
ringcharbuff_t *ringcharbuff_init(size_t size,int *err);
int ringcharbuff_empty(ringcharbuff_t *r);
int ringcharbuff_print(ringcharbuff_t *r);
int ringcharbuff_used(ringcharbuff_t *r);
int ringcharbuff_size(ringcharbuff_t *r);
int ringcharbuff_free(ringcharbuff_t *r);
int ringcharbuff_add(ringcharbuff_t *r,char *data,size_t nbytes);
int ringcharbuff_get(ringcharbuff_t *r,char *data,size_t nbytes);
int ringcharbuff_dec(ringcharbuff_t *r,size_t nbytes);
int ringcharbuff_resize(ringcharbuff_t *r,size_t nbytes,int *err);
int ringcharbuff_clear(ringcharbuff_t *r);
int ringcharbuff_hasline(ringcharbuff_t *r);
int socktype2str(char *buff,size_t buffsize,int st);
int protocol2str(char *buff,size_t buffsize,int pf);
int affamily2str(char *buff,size_t buffsizee, int af);
int printaddrinfo(struct addrinfo *ai,char *hname,char *sname,in_port_t *port,FILE *fp);
char *getipaddrstr(struct addrinfo *ai,char *hname,in_port_t *port,socklen_t buffsize);
int get_ai_family(char *ipv); // "ipv6"=AF_INET6 "ipv4"=AF_INET else AF_UNSPEC
int get_ai_socktype(char *socktype); // "tcp"=SOCK_STREAM udp="SOCK_DGRAM"
int strnlower(char *dst,char *src,size_t n);


int socktype2str(char *buff,size_t buffsize,int st) {
    switch(st) {
        case SOCK_STREAM:
            strncpy(buff,"SOCK_STREAM",buffsize);
            break;
        case SOCK_DGRAM:
            strncpy(buff,"SOCK_DGRAM",buffsize);
            break;
        case SOCK_RAW:
            strncpy(buff,"SOCK_RAW",buffsize);
            break;
        case SOCK_RDM:
            strncpy(buff,"SOCK_RDM",buffsize);
            break;
        case SOCK_SEQPACKET:
            strncpy(buff,"SOCK_SEQPACKET",buffsize);
            break;
        default:
           strncpy(buff,"UNKNOWN",buffsize);
           break;
    }
    return 0;
}

int protocol2str(char *buff,size_t buffsize,int pf) {
    switch(pf) {
        case IPPROTO_IP:
            strncpy(buff,"IPPROTO_IP",buffsize);
            break;
        case IPPROTO_ICMP:
            strncpy(buff,"IPPROTO_ICMP",buffsize);
            break;
        case IPPROTO_IGMP:
            strncpy(buff,"IPPROTO_IGMP",buffsize);
            break;
        case IPPROTO_IPIP:
            strncpy(buff,"IPPROTO_IPIP",buffsize);
            break;
        case IPPROTO_TCP:
            strncpy(buff,"IPPROTO_TCP",buffsize);
            break;
        case IPPROTO_EGP:
            strncpy(buff,"IPPROTO_EGP",buffsize);
            break;
        case IPPROTO_PUP:
            strncpy(buff,"IPPROTO_PUP",buffsize);
            break;
        case IPPROTO_UDP:
            strncpy(buff,"IPPROTO_UDP",buffsize);
            break;
        case IPPROTO_IDP:
            strncpy(buff,"IPPROTO_IDP",buffsize);
            break;
        case IPPROTO_TP:
            strncpy(buff,"IPPROTO_TP",buffsize);
            break;
        case IPPROTO_IPV6:
            strncpy(buff,"IPPROTO_IPV6",buffsize);
            break;
        case IPPROTO_ROUTING:
            strncpy(buff,"IPPROTO_ROUTING",buffsize);
            break;
        case IPPROTO_FRAGMENT:
            strncpy(buff,"IPPROTO_FRAGMENT",buffsize);
            break;
        case IPPROTO_RSVP:
            strncpy(buff,"IPPROTO_RSVP",buffsize);
            break;
        case IPPROTO_GRE:
            strncpy(buff,"IPPROTO_GRE",buffsize);
            break;
        case IPPROTO_ESP:
            strncpy(buff,"IPPROTO_ESP",buffsize);
            break;
        case IPPROTO_AH:
            strncpy(buff,"IPPROTO_AH",buffsize);
            break;
        case IPPROTO_ICMPV6:
            strncpy(buff,"IPPROTO_ICMPV6",buffsize);
            break;
        case IPPROTO_NONE:
            strncpy(buff,"IPPROTO_NONE",buffsize);
            break;
        case IPPROTO_DSTOPTS:
            strncpy(buff,"IPPROTO_DSTOPTS",buffsize);
            break;
        case IPPROTO_MTP:
            strncpy(buff,"IPPROTO_MTP",buffsize);
            break;
        case IPPROTO_ENCAP:
            strncpy(buff,"IPPROTO_ENCAP",buffsize);
            break;
        case IPPROTO_PIM:
            strncpy(buff,"IPPROTO_PIM",buffsize);
            break;
        case IPPROTO_RAW:
            strncpy(buff,"IPPROTO_RAW",buffsize);
            break;
    }
    return 0;
}

int affamily2str(char *buff,size_t buffsize, int af) {
    switch(af) {
        case AF_UNSPEC:
            strncpy(buff,"AF_UNSPEC",buffsize);
            break;
        case AF_LOCAL:
            strncpy(buff,"AF_LOCAL||AF_UNIX||AF_FILE",buffsize);
            break;
        case AF_INET:
            strncpy(buff,"AF_INET",buffsize);
            break;
        case AF_IPX:
            strncpy(buff,"AF_IPX",buffsize);
            break;
        case AF_APPLETALK:
            strncpy(buff,"AF_APPLETALK",buffsize);
            break;
        case AF_INET6:
            strncpy(buff,"AF_INET6",buffsize);
            break;
        case AF_DECnet:
            strncpy(buff,"AF_DECnet",buffsize);
            break;
        case AF_SNA:
            strncpy(buff,"AF_SNA",buffsize);
            break;
        case AF_MAX:
            strncpy(buff,"AF_MAX",buffsize);
            break;
        default:
            strncpy(buff,"UNKNOWN",buffsize);
            break;
    }
    return 0;
}

char *getipaddrstr(struct addrinfo *ai,char *hname,in_port_t  *port,socklen_t buffsize) {
    char *ipstr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    switch(ai->ai_family) {
        case AF_INET:
            sa4 = (struct sockaddr_in *)ai->ai_addr;
            *port = ntohs(sa4->sin_port);
            ipstr = inet_ntop(AF_INET,&(sa4->sin_addr),hname,buffsize);
            return ipstr;
        case AF_INET6:
            sa6 = (struct sockaddr_in6 *)ai->ai_addr;
            *port = ntohs(sa6->sin6_port);
            ipstr = inet_ntop(AF_INET6,&(sa6->sin6_addr),hname,buffsize);
            return ipstr;
        default:
            strncpy(hname,"ERROR",buffsize);
            return -1;
    }
}

int strnlower(char *dst,char *src,size_t n){
    int i;
    int c;
    for(i=0;i<n;i++){
        if(src[i]=='\0'){
            dst[i] = '\0';
            break;
        }
        dst[i] = tolower((unsigned char)src[i]);
    }
}

int get_ai_family(char *ai_family_strin){
    char ai_family_str[STRSIZE + 1];
    strnlower(ai_family_str,ai_family_strin,STRSIZE);
    if(strncmp(ai_family_str,"ipv4",STRSIZE) == 0){
        return AF_INET;
    }else if(strncmp(ai_family_str,"ipv6",STRSIZE) == 0){
        return AF_INET6;
    }else{
        return AF_UNSPEC;
    }
}

int get_ai_socktype(char *ai_socktype_strin){
    char ai_socktype_str[STRSIZE + 1];
    strnlower(ai_socktype_str,ai_socktype_strin,STRSIZE);
    if(strncmp(ai_socktype_str,"tcp",STRSIZE) == 0){
        return SOCK_STREAM;
    }else if(strncmp(ai_socktype_str,"udp",STRSIZE) == 0){
        return SOCK_DGRAM;
    }else{
        return -1;
    }
}


int printaddrinfo(struct addrinfo *ai,char *hname,char *sname,in_port_t *port,FILE *fp) {
    char ai_familystr[STRSIZE+1];
    char ai_socktypestr[STRSIZE+1];
    char ai_protocolstr[STRSIZE+1];
    char host[STRSIZE+1];
    char serv[STRSIZE+1];

    affamily2str(ai_familystr,STRSIZE,ai->ai_family);
    socktype2str(ai_socktypestr,STRSIZE,ai->ai_socktype);
    protocol2str(ai_protocolstr,STRSIZE,ai->ai_protocol);
    fprintf(fp,"{ ");
    fprintf(fp,"ai_flags=%i ",ai->ai_flags);
    fprintf(fp,"ai_family=%i(%s) ",ai->ai_family,ai_familystr);
    fprintf(fp,"ai_socktype=%i(%s) ",ai->ai_socktype,ai_socktypestr);
    fprintf(fp,"ai_protocol=%i(%s) ",ai->ai_protocol,ai_protocolstr);
    fprintf(fp,"ai_addrlen=%i ",ai->ai_addrlen);
    fprintf(fp,"ai_cannonname=%s ",ai->ai_canonname);
    if(hname != NULL && strcmp(hname,"") !=0) {
        fprintf(fp,"host=\"%s\" ",	hname);
    }
    if(sname != NULL && strcmp(sname,"") !=0) {
        fprintf(fp,"serv=\"%s\" ",sname);
    }

    if(port != NULL) {
        fprintf(fp,"port=%i ",*port);
    }
    fprintf(fp,"}\n");
    return 0;
    }

ringcharbuff_t *ringcharbuff_init(size_t size,int *err) {
    char *data;
    ringcharbuff_t *r;
    data = (char *)malloc(sizeof(char)*size);
    if(data==NULL) {
        *err=1;
        return NULL;
    }
    r = (ringcharbuff_t *)malloc(sizeof(ringcharbuff_t));
    if(r==NULL) {
        free(data);
        *err=2;
        return NULL;
    }
    r->size =size;
    r->h = 0;
    r->used = 0;
    r->data = data;
    *err=0;
    return r;
}

int ringcharbuff_used(ringcharbuff_t *r) {
    return r->used;
}

int ringcharbuff_size(ringcharbuff_t *r) {
    return r->size;
}

int ringcharbuff_free(ringcharbuff_t *r) {
    return r->size - r->used;
}

int ringcharbuff_print(ringcharbuff_t *r) {
    int i;
    int h = r->h;
    int s = ringcharbuff_size(r);
    int u = ringcharbuff_used(r);
    int f = ringcharbuff_free(r);

    printf("    ");
    for(i=0;i<s;i++) {
        if(i==h) {
            printf("%s","h");
        }
        else {
            printf("%s"," ");
        }
    }
    printf("\n    ");

    for(i=0;i<s;i++) {
        if(i%10==0) {
            printf("%i",i/10);
        }
        else {
            printf(" ");
        }
    }
    printf("\n    ");

    for(i=0;i<s;i++) {
        printf("%i",i%10);
    }
    printf("\n    ");

    for(i=0;i<s;i++) {
        switch(r->data[i]){
            case '\n':
                printf("%c",'~');
                break;
            default:
                printf("%c",r->data[i]);
                break;
        }
    }
    printf("\n    ");
    printf("u=%i f=%i s=%i\n",u,f,s);
    return 0;
}

int ringcharbuff_hasline(ringcharbuff_t *r){
    int u = r->used;
    int di = 0;
    int ri = r->h;
    if(r->size == 0) return 0;
    while(u>0){
        if(r->data[ri]=='\n'){
            return di + 1;
        }
        ri = (ri +1)%(r->size);
        di++;
        u--;
    }
    return 0;
}


int ringcharbuff_get(ringcharbuff_t *r,char *data,size_t nbytes) {
    int u = r->used;
    int di = 0;
    int ri = r->h;
    if(r->size == 0) return 0;
    while( di < nbytes && u > 0) {
        data[di] = r->data[ri];
        ri = (ri+1)%r->size;
        di++;
        u--;
    }
    return di;
}

int ringcharbuff_add(ringcharbuff_t *r,char *data,size_t nbytes) {
    int f  = ringcharbuff_free(r);
    int di = 0; // index for parameter data
    int ri;
    if(r->size == 0) return 0;
    ri = (r->h+r->used)%r->size;
    while(di < nbytes && f>0) {
        r->data[ri] = data[di];
        ri = (ri+1)%r->size;
        di++;
        f--;
    }
    r->used += di;
    return di;
}

int ringcharbuff_dec(ringcharbuff_t *r,size_t nbytes) {
    int n = (nbytes < r->used) ? nbytes : r->used;
    if(r->size == 0) return 0;
    r->h = (r->h + n)%r->size;
    r->used -= n;
    return n;   
}

int ringcharbuff_clear(ringcharbuff_t *r){
    return ringcharbuff_dec(r,r->used);
}

int ringcharbuff_empty(ringcharbuff_t *r){
    return r->used == 0;
}

int ringcharbuff_resize(ringcharbuff_t *r,size_t new_size,int *err) {
    int ri;
    int i;
    int used;
    int size;
    char *ndata;
    char *data;
    used = r->used;
    data = r->data;
    size = r->size; 
    if(new_size < used) {
        *err = 1;
        return 0;
    }
    ndata = (char *)malloc(sizeof(char)*new_size);
    if(ndata == NULL) {
        *err = 2;
        return 0;
    }

    if(size == 0) {
        r->h = 0;
        r->size = new_size;
        free(r->data);
        r->data = ndata;
        err = 0;
        return 1;
    }

    ri = r->h;
    for(i=0;i < used;i++) {
        ndata[i] = data[ri];
        ri = (ri+1)%size;
    }

    free(data);
    r->data = ndata;
    r->h = 0;
    r->size=new_size;
    *err=0;
    return 1;
}

int usage(char *prog) {
    printf("Usage is %s <host> <port> <ipv6|ipv4|any>\n",prog);
    printf("\n");
    printf("Connect to the remote echo server and\n");
    printf("send text\n");
}

ssize_t myread(int fd,char *block,size_t blocksize,int *eof){
    ssize_t nbytes;
    *eof = 0;
    nbytes = read(fd,block,blocksize);
    if(nbytes == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            nbytes = 0;
            return nbytes;
        }else{
            printf("EOF on socket %i\n",fd);
            *eof = 1;
            nbytes;
            return nbytes;
        }
    }
    if(nbytes == 0){
        *eof=1;
        return nbytes;
    }
    return nbytes;
}

ssize_t mywrite(int fd,char *block,size_t blocksize,int *eof){
    ssize_t nbytes;
    *eof = 0;
    nbytes = write(fd,block,blocksize);
    if(nbytes == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            return 0;
        }else{
            *eof = 1;
            return 0;
        }
    }
    return nbytes;
}


int setnonblocking(int s){
    int flags;
    flags = fcntl(s,F_GETFL,0);
    fcntl(s,F_SETFL, flags | O_NONBLOCK);
}

int lookup(char *host,char *port,int ai_family,struct addrinfo **result){
    int rc;
    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = ai_family;

    rc = getaddrinfo(host,port,&hints,result);
    return rc;
}

int echo_client(int cs){
    char block[BLOCKSIZE];
    int err;
    ssize_t bytesr;
    ssize_t bytesw;
    int stat;
    int si;
    int so;
    int eof;
    size_t size;
    fd_set rs,ws;
    ringcharbuff_t *sring;
    ringcharbuff_t *cring;
    sring = ringcharbuff_init(INITRINGSIZE,&err);
    cring = ringcharbuff_init(INITRINGSIZE,&err);

    si = fileno(stdin);
    so = fileno(stdout);
    setnonblocking(cs);
    setnonblocking(si);
    setnonblocking(so);

    while(1){
        FD_ZERO(&rs);
        FD_ZERO(&ws);
        FD_SET(si,&rs);
        FD_SET(cs,&rs);
        if(ringcharbuff_used(cring)>0) {
            FD_SET(so,&ws);
        }
        if(ringcharbuff_used(sring)>0) {
            FD_SET(cs,&ws);
        }
        stat = select(cs+1,&rs,&ws,NULL,NULL);
        if(stat<0){
            sleep(1);
            continue;
        }

        if(FD_ISSET(cs,&rs)){
            bytesr = myread(cs,block,BLOCKSIZE,&eof);
            if(eof){
                printf("EOF on fd %i\n",cs);
                return -1;
            }
            while(bytesr>= ringcharbuff_free(cring)){
                size = ringcharbuff_size(cring);
                size *= 2;
                printf("increasing size of cring to %zi\n",size);
                ringcharbuff_resize(cring,size,&err);
            }
            ringcharbuff_add(cring,block,bytesr);
        }


        if(FD_ISSET(si,&rs)){
            bytesr = myread(si,block,BLOCKSIZE,&eof);
            if(eof){
                printf("EOF on fd %i\n",si);
                return -1;
            }
            while(bytesr>= ringcharbuff_free(sring)){
                size = ringcharbuff_size(sring);
                size *= 2;
                printf("increasing size of sring to %zi\n",size);
                ringcharbuff_resize(sring,size,&err);
            }
            ringcharbuff_add(sring,block,bytesr);
        }

        if(FD_ISSET(so,&ws)){
            bytesr = ringcharbuff_get(cring,block,BLOCKSIZE);
            bytesw = mywrite(so,block,bytesr,&eof);
            if(eof){
                printf("EOF of stdout\n");
                return 0;
            }
            ringcharbuff_dec(cring,bytesw);
        }

        if(FD_ISSET(cs,&ws)){
            bytesr = ringcharbuff_get(sring,block,BLOCKSIZE);
            bytesw = mywrite(cs,block,bytesr,&eof);
            if(eof){
                printf("EOF of stdout\n");
                return 0;
            }
            ringcharbuff_dec(sring,bytesw);
        }
    }
}

int sockconnect(struct addrinfo *res,char *host,in_port_t *uport,FILE *fp){
    struct addrinfo *cai;
    int s = -1;
    cai = res;
    while(cai != NULL){
        getipaddrstr(cai,host,uport,STRSIZE);
        printf("Connect \n");
        printaddrinfo(cai,host,NULL,NULL,fp);
        s = socket(cai->ai_family,cai->ai_socktype,cai->ai_protocol);
        if(s == -1){
            perror("socket create failed");
            return -1;
        }
        if(connect(s,cai->ai_addr,cai->ai_addrlen)==-1) {
            perror("Connect Fail");
            close(s);
            s = -1;
            cai = cai -> ai_next;
            continue;
        }else{
            return s;
        }
    }
    fprintf(stderr,"No more addrinfo to connect to\n");
    return -1;
}

int main(int argc, char **argv){
    struct addrinfo *res;
    struct addrinfo *caddr;
    char hname[STRSIZE + 1];
    char *ipv;
    int ai_family;
    char *host;
    char *port;
    in_port_t uport;
    int rc;
    int cs;
    if(argc<3){
        usage(argv[0]);
        return 0;
    }

    host = argv[1];
    port = argv[2];
    ipv = argv[3];
    uport = atoi(port);

    fprintf(stderr,"mypid = %i\n",getpid());

    ai_family = get_ai_family(ipv);
    rc = lookup(host,port,ai_family,&res);
        if(rc != 0) {
        fprintf(stderr,"Error getting addrinfo for %s:",host);
        fprintf(stderr,"%s\n",gai_strerror(rc));
        exit(1);
        }
    cs = sockconnect(res,host,&uport,stdout);
    freeaddrinfo(res);
    echo_client(cs);
    close(cs);
    return 0;
}


