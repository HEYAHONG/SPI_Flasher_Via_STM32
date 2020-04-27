#include "RC.h"

//资源信息结构体
typedef struct
{
size_t data_offset;
size_t data_size;
size_t name_offset;
size_t name_size;
} RC_Info_t;

extern const unsigned char RC_Data[];
extern const unsigned char RC_Name[];
extern const RC_Info_t RC_Info[];
extern const size_t    RC_Info_Size;

//当待比较的数据相同时返回1
static unsigned char NameCmp(const char * Name1,const char *Name2)
{
    unsigned char ret=1;
    size_t i=0;
    while(1)
    {
        if(Name1[i]!=Name2[i])
            ret=0;
        if(Name1[i]=='\0' || Name2[i]=='\0')
            break;
        i++;
    }
    return ret;
}

//获取指定名称的资源的RC_Info,失败返回NULL
static const RC_Info_t * FindRCInfoByName(const char * Name)
{
    const RC_Info_t *ret=NULL;
    size_t i=0;
    for(i=0;i<RC_Info_Size;i++)
    {
        if(NameCmp(Name,(const char *)&RC_Name[RC_Info[i].name_offset]))
        {
            ret=&RC_Info[i];
            break;
        }
    }

    return ret;
}


//通过名称获取资源大小
size_t RCGetSize(const char * name)
{
    size_t ret=0;
    const RC_Info_t * info=FindRCInfoByName(name);
    if(info!=NULL)
    {
        ret=info->data_size;
    }
    return ret;
}

//通过名称获取资源指针
const unsigned char * RCGetHandle(const char * name)
{
    const unsigned char * ret=NULL;
    const RC_Info_t * info=FindRCInfoByName(name);
    if(info!=NULL)
    {
        ret=&RC_Data[info->data_offset];
    }
    return ret;
}



const size_t    RC_Info_Size=1;
const RC_Info_t RC_Info[]=
{
//D:/project_in_git/VirtualFat/src/FAT16_DBR.bin
{0,512,0,14}
};
const unsigned char RC_Data[]=
{
//D:/project_in_git/VirtualFat/src/FAT16_DBR.bin
0Xeb,0X3c,0X90,0X4d,0X53,0X44,0X4f,0X53,0X35,0X2e,0X30,0X0,0X2,0X1,0X4,0X0,
0X2,0X0,0X2,0X0,0X0,0Xf8,0Xfe,0X0,0X3f,0X0,0X10,0X0,0X1,0X0,0X0,0X0,
0X1,0X0,0X1,0X0,0X80,0X0,0X29,0X42,0Xba,0Xed,0Xba,0X4e,0X4f,0X20,0X4e,0X41,
0X4d,0X45,0X20,0X20,0X20,0X20,0X46,0X41,0X54,0X31,0X36,0X20,0X20,0X20,0X33,0Xc9,
0X8e,0Xd1,0Xbc,0Xf0,0X7b,0X8e,0Xd9,0Xb8,0X0,0X20,0X8e,0Xc0,0Xfc,0Xbd,0X0,0X7c,
0X38,0X4e,0X24,0X7d,0X24,0X8b,0Xc1,0X99,0Xe8,0X3c,0X1,0X72,0X1c,0X83,0Xeb,0X3a,
0X66,0Xa1,0X1c,0X7c,0X26,0X66,0X3b,0X7,0X26,0X8a,0X57,0Xfc,0X75,0X6,0X80,0Xca,
0X2,0X88,0X56,0X2,0X80,0Xc3,0X10,0X73,0Xeb,0X33,0Xc9,0X8a,0X46,0X10,0X98,0Xf7,
0X66,0X16,0X3,0X46,0X1c,0X13,0X56,0X1e,0X3,0X46,0Xe,0X13,0Xd1,0X8b,0X76,0X11,
0X60,0X89,0X46,0Xfc,0X89,0X56,0Xfe,0Xb8,0X20,0X0,0Xf7,0Xe6,0X8b,0X5e,0Xb,0X3,
0Xc3,0X48,0Xf7,0Xf3,0X1,0X46,0Xfc,0X11,0X4e,0Xfe,0X61,0Xbf,0X0,0X0,0Xe8,0Xe6,
0X0,0X72,0X39,0X26,0X38,0X2d,0X74,0X17,0X60,0Xb1,0Xb,0Xbe,0Xa1,0X7d,0Xf3,0Xa6,
0X61,0X74,0X32,0X4e,0X74,0X9,0X83,0Xc7,0X20,0X3b,0Xfb,0X72,0Xe6,0Xeb,0Xdc,0Xa0,
0Xfb,0X7d,0Xb4,0X7d,0X8b,0Xf0,0Xac,0X98,0X40,0X74,0Xc,0X48,0X74,0X13,0Xb4,0Xe,
0Xbb,0X7,0X0,0Xcd,0X10,0Xeb,0Xef,0Xa0,0Xfd,0X7d,0Xeb,0Xe6,0Xa0,0Xfc,0X7d,0Xeb,
0Xe1,0Xcd,0X16,0Xcd,0X19,0X26,0X8b,0X55,0X1a,0X52,0Xb0,0X1,0Xbb,0X0,0X0,0Xe8,
0X3b,0X0,0X72,0Xe8,0X5b,0X8a,0X56,0X24,0Xbe,0Xb,0X7c,0X8b,0Xfc,0Xc7,0X46,0Xf0,
0X3d,0X7d,0Xc7,0X46,0Xf4,0X29,0X7d,0X8c,0Xd9,0X89,0X4e,0Xf2,0X89,0X4e,0Xf6,0Xc6,
0X6,0X96,0X7d,0Xcb,0Xea,0X3,0X0,0X0,0X20,0Xf,0Xb6,0Xc8,0X66,0X8b,0X46,0Xf8,
0X66,0X3,0X46,0X1c,0X66,0X8b,0Xd0,0X66,0Xc1,0Xea,0X10,0Xeb,0X5e,0Xf,0Xb6,0Xc8,
0X4a,0X4a,0X8a,0X46,0Xd,0X32,0Xe4,0Xf7,0Xe2,0X3,0X46,0Xfc,0X13,0X56,0Xfe,0Xeb,
0X4a,0X52,0X50,0X6,0X53,0X6a,0X1,0X6a,0X10,0X91,0X8b,0X46,0X18,0X96,0X92,0X33,
0Xd2,0Xf7,0Xf6,0X91,0Xf7,0Xf6,0X42,0X87,0Xca,0Xf7,0X76,0X1a,0X8a,0Xf2,0X8a,0Xe8,
0Xc0,0Xcc,0X2,0Xa,0Xcc,0Xb8,0X1,0X2,0X80,0X7e,0X2,0Xe,0X75,0X4,0Xb4,0X42,
0X8b,0Xf4,0X8a,0X56,0X24,0Xcd,0X13,0X61,0X61,0X72,0Xb,0X40,0X75,0X1,0X42,0X3,
0X5e,0Xb,0X49,0X75,0X6,0Xf8,0Xc3,0X41,0Xbb,0X0,0X0,0X60,0X66,0X6a,0X0,0Xeb,
0Xb0,0X42,0X4f,0X4f,0X54,0X4d,0X47,0X52,0X20,0X20,0X20,0X20,0Xd,0Xa,0X52,0X65,
0X6d,0X6f,0X76,0X65,0X20,0X64,0X69,0X73,0X6b,0X73,0X20,0X6f,0X72,0X20,0X6f,0X74,
0X68,0X65,0X72,0X20,0X6d,0X65,0X64,0X69,0X61,0X2e,0Xff,0Xd,0Xa,0X44,0X69,0X73,
0X6b,0X20,0X65,0X72,0X72,0X6f,0X72,0Xff,0Xd,0Xa,0X50,0X72,0X65,0X73,0X73,0X20,
0X61,0X6e,0X79,0X20,0X6b,0X65,0X79,0X20,0X74,0X6f,0X20,0X72,0X65,0X73,0X74,0X61,
0X72,0X74,0Xd,0Xa,0X0,0X0,0X0,0X0,0X0,0X0,0X0,0Xac,0Xcb,0Xd8,0X55,0Xaa
};
const unsigned char RC_Name[]=
{
//D:/project_in_git/VirtualFat/src/FAT16_DBR.bin
0X46,0X41,0X54,0X31,0X36,0X5f,0X44,0X42,0X52,0X2e,0X62,0X69,0X6e,0x0
};
