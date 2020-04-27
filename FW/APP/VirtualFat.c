/*
 * VirtualFat.c
 *
 *  Created on: Apr 20, 2020
 *      Author: Administrator
 */
#include "VirtualFat.h"
#include "RC.H"

//fat文件系统各部分起始扇区地址

static uint32_t FAT1_START=4;//FAT表
static uint32_t FAT2_START=258;
static uint32_t	RootDir_START=512;//根目录
static uint32_t FileData_START=544;//文件数据

//fat虚拟文件数据
static VirtualFat_File * root_file_list[FAT16_Root_File_Number]={};

#ifdef FAT16_FSINFO_TXT
//显示VirtualFat信息文件fsinfo.txt

static int8_t errorlevel=0;

static void fsinfo_txt_read(uint8_t *buf,size_t offset,size_t length)
{
	//memset(buf,' ',length);
	{//添加UTF-8 BOM头,以便在windows下打开
		buf[0]=0xef;
		buf[1]=0xbb;
		buf[2]=0xbf;
	}
	sprintf((char *)buf+3,
"\
Virtual FSType:%s\r\n\
Author:%s\r\n\
ErrorLevel:%d\r\n\
命令使用方式:\r\n\
    在cmd控制台中进入虚拟优盘目录并键入: echo 命令  > FSINFO.TXT\r\n\
当前可用的命令:\r\n\
RESET\t重启单片机\n\r\
		","FAT16","何亚红",errorlevel);
	{//用空格填充文本，防止在某些文本编辑器上出现异常
		for(size_t i=0;i<length;i++)
		{
			if(buf[i]=='\0')
				buf[i]=' ';
		}
	}

}

//成功返回1,比较是否写入某命令
static uint8_t fsinfo_cmd_compare(uint8_t *buf,uint8_t *cmd)
{
	uint8_t ret=1;
	for(size_t i=0;i<strlen((char *)cmd);i++)
	{
		if(buf[i]!=cmd[i])
			ret=0;
	}
	return ret;
}

static void fsinfo_txt_write(uint8_t *buf,size_t offset,size_t length)
{
	errorlevel=-1;
	if(buf[0]==0)
	{
		errorlevel=-2;
	}
	else
	{
		if(fsinfo_cmd_compare(buf,(uint8_t *)"RESET"))//执行复位指令
		{
			errorlevel=0;
			{
//STM32F103复位函数
#if __CORTEX_M==3
				NVIC_SystemReset();
#endif
			}
		}
	}
}

static VirtualFat_File fsinfo=
{
{'F','S','I','N','F','O'},
{'T','X','T'},
fsinfo_txt_read,
fsinfo_txt_write,
512
};

#endif

static void init_fatfs(FAT_BPB *bpb)
{
	//以下均是FAT16的计算方式
	FAT1_START=bpb->BPB_RsvdSecCnt[0]+bpb->BPB_RsvdSecCnt[1]*256;
	FAT2_START=FAT1_START+bpb->BPB_FATSz16[0]+bpb->BPB_FATSz16[1]*256;
	RootDir_START=FAT2_START*2-FAT1_START;
	FileData_START=RootDir_START+(bpb->BPB_RootEntCnt[0]+bpb->BPB_RootEntCnt[1]*256)*32/(bpb->BPB_BytsPerSec[0]+bpb->BPB_BytsPerSec[1]*256);
#ifdef FAT16_FSINFO_TXT
	VirtualFat_Unregister_RootFile(&fsinfo);
	VirtualFat_Register_RootFile(&fsinfo);
#endif
}

//处理FAT表
//fat_offset为fat表中的扇区偏移
static void read_fat_fatfs(uint32_t fat_offset,uint8_t *buf)
{
	if(fat_offset==0)
	{
		{//处理特殊的项
			//FAT表第一项与第二项
			buf[0]=0xf8;
			buf[1]=0xff;
			buf[2]=0xff;
			buf[3]=0xff;
		}
	}
	{//填写文件的FAT表
		size_t fat_start_index=	512*fat_offset/2;//FAT16计算当前FAT扇区起始的簇号
		size_t file_start=2;//文件起始簇号
		for(size_t i=0;i<FAT16_Root_File_Number;i++)
		{
			if(root_file_list[i]!=NULL && root_file_list[i]->size!=0)//虚拟文件有效
			{
				//计算FAT占用的簇大小
				size_t file_size=root_file_list[i]->size/512;
				size_t file_current=0;//当前的簇
				if(root_file_list[i]->size%512 !=0)
				{
					file_size+=1;
				}
				for(file_current=0;file_current<file_size;file_current++)
				{

					if(file_start>=fat_start_index)//文件的FAT在当前扇区有数据
					{
						if((file_start+file_current-fat_start_index)<512/2)//文件数据未超出当前扇区
						{
							if(file_current!=file_size-1)//是否为文件结束
							{//指向下一簇
								buf[(file_start+file_current-fat_start_index)*2]=(file_start+1)&0xff;
								buf[(file_start+file_current-fat_start_index)*2+1]=((file_start+1)&0xff00)>>8;
							}
							else
							{//文件结束
								buf[(file_start+file_current-fat_start_index)*2]=0xff;
								buf[(file_start+file_current-fat_start_index)*2+1]=0xff;
							}
						}
					}

				}
				file_start+=file_size;


			}
		}
	}
}


//处理根目录
//rootdir_offset为扇区偏移
static void read_rootdir_fatfs(uint32_t rootdir_offset,uint8_t *buf)
{
	if(rootdir_offset==0)
	{
		{//设置卷标
			FAT_DIR *p=(FAT_DIR *)&(buf[0]);
			{
				//文件名用0x20填充
				memset(p->DIR_Name,0x20,sizeof(p->DIR_Name));
				//设置文件名
				memcpy(p->DIR_Name,FAT16_Volume_ID,sizeof(FAT16_Volume_ID)>11?11:sizeof(FAT16_Volume_ID));

				p->DIR_Att=0x08;

				p->DIR_WrtDat[0]=0x94;
				p->DIR_WrtDat[1]=0x50;
				p->DIR_WrtTime[0]=0x29;
				p->DIR_WrtTime[1]=0x53;

			}

		}

	}
	{//填写文件
		size_t rootdir_start_index=512*rootdir_offset/32;//当前扇区文件偏移
		size_t rootdir_start=1;//起始文件偏移
		size_t fat_start=2;//起始文件簇
		for(size_t i=0;i<FAT16_Root_File_Number;i++)
		{

			if(root_file_list[i]==NULL || root_file_list[i]->size==0)
				continue;

			if(rootdir_start>=rootdir_start_index)//文件属于当前扇区
			{
				if((rootdir_start-rootdir_start_index)<512/32)//文件未超出当前扇区
				{
					FAT_DIR *p=(FAT_DIR *)&(buf[(rootdir_start-rootdir_start_index)*32]);
					{//设置文件属性
						//设置文件名
						memset(p->DIR_Name,0x20,11);

						{//设置文件名
							for(uint8_t j=0;j<11;j++)
							{
								if(j<8)
								{
									if(root_file_list[i]->FileName[j]!='\0')
									{
										p->DIR_Name[j]=root_file_list[i]->FileName[j];
									}
								}
								else
								{
									if(root_file_list[i]->FileName_sub[j-8]!='\0')
									{
											p->DIR_Name[j]=root_file_list[i]->FileName_sub[j-8];
									}
								}
							};
						}
						{//设置文件属性
							p->DIR_Att=0x20;

							//p->DIR_NTRes=0x18;

							p->DIR_CrtTimeTenth=0x7f;

							p->DIR_CrtTime[0]=0xa2;

							p->DIR_CrtTime[1]=0x58;

							p->DIR_CrtDate[0]=0x95;

							p->DIR_CrtDate[1]=0x50;

							p->DIR_WrtTime[0]=0x71;
							p->DIR_WrtTime[1]=0x5a;

							memcpy(p->DIR_LstAccDate,p->DIR_CrtDate,2);
							memcpy(p->DIR_WrtDat,p->DIR_CrtDate,2);


						}

						{//设置文件起始簇
							p->DIR_FstClusLO[0]=fat_start&0xff;
							p->DIR_FstClusLO[1]=(fat_start&0xff00)>>8;
						}
						{//设置文件大小
							memcpy(p->DIR_FileSize,&root_file_list[i]->size,4);
						}

					}
				}
			}
			{//增加fat计数
				size_t fat_size=root_file_list[i]->size/512;
				if(root_file_list[i]->size%512!=0)
					fat_size+=1;
				fat_start+=fat_size;
			}
			rootdir_start++;
		}
	}

}

//处理数据文件
//filedata_offset为扇区偏移

static void read_filedata_fatfs(uint32_t filedata_offset,uint8_t *buf)
{
	size_t file_blk_start=0;//文件数据区起始
	for(size_t i=0;i<FAT16_Root_File_Number;i++)
	{
		if(root_file_list[i]!=NULL && root_file_list[i]->size!=0)
		{
			size_t file_blk_size=root_file_list[i]->size/512;
			if(root_file_list[i]->size%512 !=0)
				file_blk_size++;
			if(file_blk_start<=filedata_offset && (file_blk_start+file_blk_size)>filedata_offset)
			{//此块在文件内
				if(root_file_list[i]->read !=NULL)
				{
					root_file_list[i]->read(buf,512*(filedata_offset-file_blk_start),512);
				}
			}
			file_blk_start+=file_blk_size;
		}
	}
}

static void write_filedata_fatfs(uint32_t filedata_offset,uint8_t *buf)
{

size_t file_blk_start=0;//文件数据区起始
	for(size_t i=0;i<FAT16_Root_File_Number;i++)
	{
		if(root_file_list[i]!=NULL && root_file_list[i]->size!=0)
		{
			size_t file_blk_size=root_file_list[i]->size/512;
			if(root_file_list[i]->size%512 !=0)
				file_blk_size++;
			if(file_blk_start<=filedata_offset && (file_blk_start+file_blk_size)>filedata_offset)
			{//此块在文件内
				if(root_file_list[i]->write !=NULL)
				{
					root_file_list[i]->write(buf,512*(filedata_offset-file_blk_start),512);
				}
			}
			file_blk_start+=file_blk_size;
		}
	}
}

//定义初始化函数，一般由STORAGE_Init_FS调用
uint8_t VirtualFat_Init(uint8_t lun)
{
	//加载FAT16_DBR.bin
	if(RCGetSize("FAT16_DBR.bin"))
	{
		init_fatfs((FAT_BPB *)RCGetHandle("FAT16_DBR.bin"));
	}
	return 0;
}
//定义读函数,一般由STORAGE_Read_FS调用
uint8_t VirtualFat_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	//清空buf
	memset(buf,0,blk_len*STORAGE_BLK_SIZ);

	//处理引导扇区与保留扇区
	if(blk_addr<FAT1_START)
	{  //返回DBR
		if(blk_addr==0)
		{
		//加载FAT16_DBR.bin
		if(RCGetSize("FAT16_DBR.bin"))
			{
				memcpy(buf,RCGetHandle("FAT16_DBR.bin"),RCGetSize("FAT16_DBR.bin"));
			}
		}
	}

	//处理FAT表
	if(blk_addr>=FAT1_START && blk_addr<RootDir_START)
	{
		uint32_t fat_offset=blk_addr-FAT1_START;
		if(blk_addr>=FAT2_START)
				 fat_offset-=(FAT2_START-FAT1_START);
		read_fat_fatfs(fat_offset,buf);
	}

	//处理根目录
	if(blk_addr >=RootDir_START && blk_addr <FileData_START)
	{
		read_rootdir_fatfs(blk_addr-RootDir_START,buf);
	}

	//处理文件数据
	if(blk_addr>=FileData_START)
	{
		 read_filedata_fatfs(blk_addr-FileData_START,buf);
	}
	return 0;
}

//定义写函数，一般由STORAGE_Write_FS调用
uint8_t VirtualFat_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	//处理文件数据
	if(blk_addr>=FileData_START)
	{
		write_filedata_fatfs(blk_addr-FileData_START,buf);
	}
	return 0;
}


//注册与反注册根目录文件
//成功返回1
//file指针所指的文件必须长期有效，不可使用局部非静态变量
uint8_t VirtualFat_Register_RootFile(VirtualFat_File * file)
{
	for(size_t i=0;i<FAT16_Root_File_Number;i++)
	{
		if(root_file_list[i]==NULL)
		{
			root_file_list[i]=file;
			return 1;
		}
	}
	return 0;
}
uint8_t VirtualFat_Unregister_RootFile(VirtualFat_File * file)
{
	for(size_t i=0;i<FAT16_Root_File_Number;i++)
		{
			if(root_file_list[i]==file)
			{
				root_file_list[i]=NULL;
				return 1;
			}
		}
	return 0;
}
