#pragma once
//#define NOREAD true
// TAZA Build Information

//#define DETAILEDLOGGING
#define TAZAVERSION "2.0.1"
#define TAZAEXTRAS (NOEXTRAS ? false : true)
#define TAZADEFAULTIV "TAZ_DEFAULT_IV";
#define TAZADEFAULTKEY "TAZ_DEFAULT_KEY";
#define TAZADEFAULTWARN "This is the default message for TAZA V" TAZAVERSION "! This is intended as an pleaceholder, please update your V2Archive for your application.\n"
#define TAZPLAINDATA
#ifdef NOWRITE 
#define COMPRESS_H
#define ENCRYPT_H
#define MTFP_H
#endif
#ifdef NOREAD 
#define DECOMPRESS_H
#define DECRYPT_H
#endif