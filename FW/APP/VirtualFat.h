/*
 * VirtualFat.h
 *
 *  Created on: Apr 20, 2020
 *      Author: Administrator
 */

#ifndef SRC_VIRTUALFAT_H_
#define SRC_VIRTUALFAT_H_

#include "stdint.h"
#include "string.h"

//包含STM32库函数,错误时可取消包含
#include "main.h"

/*
 * 虚拟的磁盘大小为32M,扇区大小512字节，FAT16簇大小512字节，请在充分理解下述代码时修改基础参数。
 */

//导入资源文件
#include "RC.h"


#define UNUSED_PARA(x) ((void)x)

//定义存储的块数量，需要与usbd_storage_if.c中的定义保持一致，总容量必须为32M
#ifndef STORAGE_BLK_NBR
#define STORAGE_BLK_NBR                  0x10000
#endif

//定义存储的块大小，需要与usbd_storage_if.c中的定义保持一致，必须为512
#ifndef STORAGE_BLK_SIZ
#define STORAGE_BLK_SIZ                  0x200
#endif

//使用FAT16
#define FAT16

//定义放置在FAT16根文件的数量，此参数受到FAT16根文件数量限制，且限制注册虚拟文件的数量
#define FAT16_Root_File_Number 4

//定义卷标（小于11字符）
#define FAT16_Volume_ID "Flasher"

//显示VirtualFat信息文件fsinfo.txt
#define FAT16_FSINFO_TXT

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef FAT16
#error "暂时只支持FAT16"
#endif

//定义数据类型,仅支持小端模式CPU
//定义BPB结构
typedef struct FAT_BPB_T
{
uint8_t BS_jmpBoot[3];
uint8_t BS_OEMName[8];
uint8_t BPB_BytsPerSec[2];
uint8_t BPB_SecPerClus;
uint8_t BPB_RsvdSecCnt[2];
uint8_t BPB_NumFATs;
uint8_t BPB_RootEntCnt[2];
uint8_t BPB_TotSec16[2];
uint8_t BPB_Media;
uint8_t BPB_FATSz16[2];
uint8_t BPB_SecPerTrk[2];
uint8_t BPB_NumHeads[2];
uint8_t BPB_HiddSec[4];
uint8_t BPB_TotSec32[4];

#if defined(FAT12) || defined(FAT16)
uint8_t BS_DrvNum;
uint8_t BS_Reserved1;
uint8_t BS_BootSig;
uint8_t BS_VolID[4];
uint8_t BS_VolLab[11];
uint8_t BS_FilSysType[8];

#else
uint8_t BPB_FATSz32[4];
uint8_t BPB_ExtFlags[2];
uint8_t BPB_FSVer[2];
uint8_t BPB_RootClus[4];
uint8_t BPB_FSInfo[2];
uint8_t BPB_BkBootSec[2];
uint8_t BPB_Reserved[12];
uint8_t BS_DrvNum;
uint8_t BS_Reserved1;
uint8_t BS_BootSig;
uint8_t BS_VolID[4];
uint8_t BS_VolLab[11];
uint8_t BS_FilSysType[8];
#endif


} FAT_BPB;


typedef struct FAT_DIR_t
{
uint8_t	DIR_Name[11];
uint8_t	DIR_Att;
uint8_t	DIR_NTRes;
uint8_t	DIR_CrtTimeTenth;
uint8_t	DIR_CrtTime[2];
uint8_t	DIR_CrtDate[2];
uint8_t	DIR_LstAccDate[2];
uint8_t	DIR_FstClusHI[2];
uint8_t	DIR_WrtTime[2];
uint8_t	DIR_WrtDat[2];
uint8_t	DIR_FstClusLO[2];
uint8_t	DIR_FileSize[4];
} FAT_DIR;

//文件读写操作回调函数类型，offset为在文件中的偏移，length最大读写长度
typedef void (*VirtualFat_RW_Op)(uint8_t *buf,size_t offset,size_t length);

//虚拟文件，使用此类型实现数据交互
//文件名需大写，小写肯可能会导致无法打开
typedef struct VirtualFat_File_t
{
char FileName[8];
char FileName_sub[3];//文件后缀
VirtualFat_RW_Op read;//读回调
VirtualFat_RW_Op write;//写回调
size_t size;
} VirtualFat_File;



//定义初始化函数，一般由STORAGE_Init_FS调用
uint8_t VirtualFat_Init(uint8_t lun);

//定义读函数,一般由STORAGE_Read_FS调用
uint8_t VirtualFat_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

//定义写函数，一般由STORAGE_Write_FS调用
uint8_t VirtualFat_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

//注册与反注册根目录文件
//成功返回1
//file指针所指的文件必须长期有效，不可使用局部非静态变量
uint8_t VirtualFat_Register_RootFile(VirtualFat_File * file);
uint8_t VirtualFat_Unregister_RootFile(VirtualFat_File * file);


#ifdef __cplusplus
};
#endif

#endif /* SRC_VIRTUALFAT_H_ */
