/*
 * Kouler v0.1.0-source
 * Bounded STOS caller for ABBY atomic state replacement.
 * No caller-supplied target is accepted.
 */
#include "kouler.h"
#include "abby.h"
#include <stdlib.h>
#include <string.h>

#define KOULER_SCOPE_ID "xkyoot.static"
#define KOULER_RELATIVE_OBJECT "ui/mini-cloud.html"
#ifndef KOULER_STOS_HOME
#error "KOULER_STOS_HOME must be bound by the authorized sovereign STOS build"
#endif
#define KOULER_NATIVE_OBJECT KOULER_STOS_HOME "/runtime/storage/xkyoot.static/ui/mini-cloud.html"

static const unsigned char PREIMAGE[] =
"#tree{\n"
"  display:none !important;\n"
"}\n";
static const unsigned char V5_INERT[] =
"<script id=\"stos-organized-v5-js\" type=\"application/x-stos-preserved\">";
static const unsigned char V5_ACTIVE[] =
"<script id=\"stos-organized-v5-js\">";
static const unsigned char DIRECTORY_MARKER[] =
"const DIRECTORY_ENDPOINT=\"/v1/directory\";";

static size_t count_exact(const unsigned char *b,size_t n,
                          const unsigned char *q,size_t m){
    size_t i,c=0;
    if(!b||!q||!m||n<m)return 0;
    for(i=0;i+m<=n;i++)if(memcmp(b+i,q,m)==0){c++;i+=m-1;}
    return c;
}
static int find_once(const unsigned char*b,size_t n,
                     const unsigned char*q,size_t m,size_t*at){
    size_t i,c=0,p=0;
    if(!at)return 0;
    for(i=0;i+m<=n;i++)if(memcmp(b+i,q,m)==0){c++;p=i;i+=m-1;}
    if(c!=1)return 0; *at=p; return 1;
}
static kouler_status read_all(abby_storage_scope *scope,
                              unsigned char **out,size_t *out_n){
    abby_storage_info info;
    abby_file *f=NULL;
    unsigned char *b=NULL;
    size_t total=0,got=0;
    abby_status s;
    *out=NULL; *out_n=0;
    s=abby_storage_stat(scope,KOULER_RELATIVE_OBJECT,&info);
    if(s!=ABBY_OK||info.size==0)return KOULER_EIO;
    b=(unsigned char*)malloc((size_t)info.size+1u);
    if(!b)return KOULER_EIO;
    s=abby_storage_file_open(scope,KOULER_RELATIVE_OBJECT,
        ABBY_FILE_READ,ABBY_FILE_PROTECTION_PRIVATE,&f);
    if(s!=ABBY_OK){free(b);return KOULER_EIO;}
    while(total<(size_t)info.size){
        got=0;
        s=abby_file_read(f,b+total,(size_t)info.size-total,&got);
        if(s!=ABBY_OK&&s!=ABBY_EOF){(void)abby_file_close(&f);free(b);return KOULER_EIO;}
        total+=got;
        if(s==ABBY_EOF)break;
        if(!got){(void)abby_file_close(&f);free(b);return KOULER_EIO;}
    }
    (void)abby_file_close(&f);
    if(total!=(size_t)info.size){free(b);return KOULER_EIO;}
    b[total]=0; *out=b; *out_n=total; return KOULER_OK;
}
kouler_status kouler_main_directory_recovery(kouler_receipt *r){
    abby_storage_scope *scope=NULL;
    unsigned char *src=NULL,*post=NULL,*verify=NULL;
    size_t sn=0,pn=0,vn=0,cut=0,pre=sizeof(PREIMAGE)-1u;
    abby_status as;
    kouler_status ks=KOULER_EIO;
    if(!r)return KOULER_EINVAL;
    memset(r,0,sizeof(*r));
    as=abby_storage_scope_open_existing(KOULER_SCOPE_ID,&scope);
    if(as!=ABBY_OK)return KOULER_EAUTHORITY;
    ks=read_all(scope,&src,&sn); if(ks!=KOULER_OK)goto done;
    if(!find_once(src,sn,PREIMAGE,pre,&cut) ||
       count_exact(src,sn,V5_INERT,sizeof(V5_INERT)-1u)!=1u ||
       count_exact(src,sn,V5_ACTIVE,sizeof(V5_ACTIVE)-1u)!=0u ||
       count_exact(src,sn,DIRECTORY_MARKER,sizeof(DIRECTORY_MARKER)-1u)!=1u){
        ks=KOULER_EPREIMAGE; goto done;
    }
    r->preimage_verified=1u; r->source_bytes=sn;
    pn=sn-pre;
    post=(unsigned char*)malloc(pn+1u); if(!post){ks=KOULER_EIO;goto done;}
    memcpy(post,src,cut);
    memcpy(post+cut,src+cut+pre,sn-cut-pre);
    post[pn]=0;
    if(count_exact(post,pn,PREIMAGE,pre)!=0u ||
       count_exact(post,pn,V5_INERT,sizeof(V5_INERT)-1u)!=1u ||
       count_exact(post,pn,V5_ACTIVE,sizeof(V5_ACTIVE)-1u)!=0u ||
       count_exact(post,pn,DIRECTORY_MARKER,sizeof(DIRECTORY_MARKER)-1u)!=1u){
        ks=KOULER_EPOSTIMAGE; goto done;
    }
    r->output_bytes=pn; r->removed_bytes=pre;
    as=abby_state_replace_private(KOULER_NATIVE_OBJECT,post,pn);
    if(as!=ABBY_OK){ks=KOULER_EIO;goto done;}
    r->replacement_committed=1u;
    ks=read_all(scope,&verify,&vn); if(ks!=KOULER_OK)goto done;
    if(vn!=pn||memcmp(verify,post,pn)!=0){ks=KOULER_EPOSTIMAGE;goto done;}
    r->postimage_verified=1u; ks=KOULER_OK;
done:
    if(verify){abby_secure_zero(verify,vn);free(verify);}
    if(post){abby_secure_zero(post,pn);free(post);}
    if(src){abby_secure_zero(src,sn);free(src);}
    if(scope)(void)abby_storage_scope_close(&scope);
    return ks;
}
