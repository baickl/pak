#include "pak/pak.h"
#include "crc32/crc32.h"
#include <stdio.h>
#include <memory.h>

pak* pak_open(const char* _pakfile)
{
	int i;
	pak *_pak;
	FILE *fp;
	pak_header header;
	pak_iteminfo*iteminfos;
	int filenamelen;

	/*
	 * 打开文件
	 * */
	fp = fopen(_pakfile,"rb");
	if( !fp )
		return NULL;

	/* 
	 * 校验HEADER头
	 * */
	if( fread(&header,sizeof(header),1,fp) != sizeof(header))
	{
		fclose(fp);
		return NULL;
	}

	if( header._M_flag != 'pakx')
	{
		fclose(fp);
		return NULL;
	}

	if( header._M_version > 1 )
	{
		fclose(fp);
		return NULL;
	}

	if( header._M_count <= 0 )
	{
		fclose(fp);
		return NULL;
	}
	

	/* 
	 * 提取文件信息
	 * */

	iteminfos = (pak_iteminfo*)malloc(header._M_count*sizeof(pak_iteminfo));
	if( !iteminfos )
	{
		fclose(fp);
		return NULL;
	}

	for( i = 0; i<header._M_count; ++i )
	{


		if( fread(&iteminfos[i]._M_offset,sizeof(iteminfos[i]._M_offset),1,fp) != sizeof(iteminfos[i]._M_offset))
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}

		iteminfos[i]._M_offset += header._M_offset; 

		if( fread(&iteminfos[i]._M_size,sizeof(iteminfos[i]._M_size),1,fp) != sizeof(iteminfos[i]._M_size))
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}

		if( fread(&iteminfos[i]._M_crc32,sizeof(iteminfos[i]._M_crc32),1,fp) != sizeof(iteminfos[i]._M_size))
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}

		filenamelen = 0;
		if( fread(&filenamelen,sizeof(filenamelen),1,fp) != sizeof(filenamelen))
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}

		if( filenamelen <= 0 || filenamelen >= MAX_FILENAME )
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}

		memset(iteminfos[i]._M_filename,0,sizeof(iteminfos[i]._M_filename));
		if( fread(iteminfos[i]._M_filename,filenamelen,1,fp) != filenamelen )
		{
			fclose(fp);
			free(iteminfos);
			return NULL;
		}
	}



	if( fp )
	{
		fclose(fp);
		fp = NULL;
	}


	/* 
	 * 读取成功，组织文件包
	 * */
	_pak = (pak*)malloc(sizeof(pak));
	if( !_pak )
	{
		free(iteminfos);
		return NULL;
	}
	memset(_pak->_M_filename,0,sizeof(_pak->_M_filename));
	memset(&_pak->_M_header,0,sizeof(_pak->_M_header));
	strcpy(_pak->_M_filename,_pakfile);
	memcpy(&_pak->_M_header,&header,sizeof(header));
	_pak->_M_iteminfos = iteminfos;

	return _pak;
}

void pak_close(pak* _pak)
{
	if( !_pak )
		return;

	if( _pak->_M_iteminfos)
		free(_pak->_M_iteminfos);

	free(_pak);
}

int pak_item_getcount(pak* _pak)
{
	if( !_pak)
		return 0;

	return _pak->_M_header._M_count;
}

pak_iteminfo* pak_item_getinfo(pak*_pak,int _index )
{
	if( !_pak)
		return NULL;

	if( _index < 0 || _index >= _pak->_M_header._M_count )
		return NULL;

	return &_pak->_M_iteminfos[_index];
}

int  pak_item_locate(pak*_pak,const char* _file)
{
	int i;

	if( !_pak || !_file)
		return -1;

	for( i = 0; i<_pak->_M_header._M_count; ++i )
	{
		if( stricmp(_file,_pak->_M_iteminfos[i]._M_filename) == 0 )
			return i;
	}

	return -1;
}

int pak_item_unpack_index( pak* _pak,int _index,void *_buf,int _bufsize)
{

	pak_iteminfo* iteminfo;
	FILE* fp;

	if( !_pak || !_buf )
		return 0;

	iteminfo = pak_get_iteninfo(_pak,_index);
	if( !iteminfo )
		return 0;

	if( iteminfo->_M_size > _bufsize )
		return 0;

	
	/* 
	 * 打开文件尝试读取数据
	 * */

	fp = fopen(_pak->_M_filename,"rb");
	if( !fp )
		return 0;

	if( fseek(fp,iteminfo->_M_offset,SEEK_SET) != iteminfo->_M_offset)
	{
		fclose(fp);
		return 0;
	}

	if( fread(_buf,iteminfo->_M_size,1,fp) != iteminfo->_M_size)
	{
		fclose(fp);
		return 0;
	}
 
	fclose(fp);
	fp = 0;

	/*
	 * 校验数据是否正确
	 * */
	unsigned int crc32 = calc_crc32(_buf,iteminfo->_M_size);
	if( crc32 != iteminfo->_M_crc32 )
	{
		return 0;
	}

	return iteminfo->_M_size;
}

int pak_item_unpack_filename(pak*_pak,const char*_file,void*_buf,int _bufsize)
{
	int index;
	pak_iteminfo* iteminfo;

	index = pak_item_locate(_pak,_file);
	if( index < 0 )
		return 0;

	return pak_item_unpack_index(_pak,index,_buf,_bufsize);
}