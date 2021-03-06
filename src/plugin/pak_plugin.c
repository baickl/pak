﻿/***********************************************************************************
 * Copyright (c) 2013, baickl(baickl@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 
 * * Neither the name of the {organization} nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************/
#include "pak_plugin.h"
#include "../pak.h"
#include <vfs/util.h>
#include <string.h>

static const VFS_CHAR* pak_plugin_archive_get_plugin_name()
{
    return "VFS_PLUGIN_PAK";
}

static const VFS_CHAR* pak_plugin_archive_get_plugin_version()
{
    return "1.0.0.1";
}

static const VFS_CHAR* pak_plugin_archive_get_plugin_description()
{
    return "VFS_PLUGIN_PAK";
}

static const VFS_CHAR* pak_plugin_archive_get_plugin_copyright()
{
    return "Copyright(C) baickl(baickl@gmail.com)";
}

static const VFS_CHAR* pak_plugin_archive_get_plugin_support()
{
    return "Email:baickl@gmail.com";
}

static VFS_BOOL pak_plugin_archive_check_type(const VFS_CHAR* archive)
{
    const VFS_CHAR* p;
    if( !archive )
        return VFS_FALSE;
    
    p = vfs_util_path_get_extension(archive);
    if( p && stricmp(p,"pak") == 0 )
        return VFS_TRUE;

    return VFS_FALSE;
}

static vfs_archive pak_plugin_archive_open(const VFS_CHAR* archive,const VFS_CHAR* prefix,const VFS_CHAR* passwd)
{
    return (vfs_archive)pak_open(archive,prefix);
}

static VFS_VOID pak_plugin_archive_close( vfs_archive archive)
{
    pak_close((pak*)archive);
}

static const VFS_CHAR* pak_plugin_archive_get_name(vfs_archive* archive)
{
    pak* _pak = (pak*)archive;
    if(_pak == NULL ) return NULL;

    return _pak->_M_filename;
}

static VFS_INT32 pak_plugin_archive_item_get_count( vfs_archive archive)
{
    return pak_item_get_count((pak*)archive);
}

static VFS_INT32 pak_plugin_item_foreach_proc(pak*_pak ,VFS_CHAR* filename, pak_iteminfo *iteminfo , int index, VFS_VOID*p )
{
    archive_item_foreach_proc proc;
    proc = (archive_item_foreach_proc)p;
    return proc((vfs_archive)_pak,filename,iteminfo->_M_size);
}

static VFS_BOOL pak_plugin_archive_item_foreach( vfs_archive archive,archive_item_foreach_proc proc)
{
    return pak_item_foreach((pak*)archive,pak_plugin_item_foreach_proc,proc);
}

static VFS_BOOL pak_plugin_archive_item_locate(vfs_archive archive,const VFS_CHAR* filename,VFS_UINT64* osize)
{
    pak_iteminfo* item;
    
    item = pak_item_locate((pak*)archive,filename);
    if( item == NULL )
        return VFS_FALSE;
    
    if( osize )
        *osize = item->_M_size;
    return VFS_TRUE;
}

static VFS_BOOL pak_plugin_archive_item_unpack_filename( vfs_archive archive, const VFS_CHAR* filename, VFS_VOID* buf, VFS_UINT64 bufsize)
{
    return pak_item_unpack_filename((pak*)archive,filename,buf,bufsize);
}


/************************************************************************/
/* 导出函数                                                    */
/************************************************************************/
vfs_plugin vfs_get_plugin_archive_pak()
{
    vfs_plugin plugin;

    memset(&plugin,0,sizeof(plugin));

    plugin.type = PLUGIN_ARCHIVE;
    plugin.info.get_plugin_name                             = pak_plugin_archive_get_plugin_name;
    plugin.info.get_plugin_version                          = pak_plugin_archive_get_plugin_version;
    plugin.info.get_plugin_description                      = pak_plugin_archive_get_plugin_description;
    plugin.info.get_plugin_copyright                        = pak_plugin_archive_get_plugin_copyright;
    plugin.info.get_plugin_support                          = pak_plugin_archive_get_plugin_support;
    
    plugin.plugin.archive.archive_check_suppert             = pak_plugin_archive_check_type;
    plugin.plugin.archive.archive_open                      = pak_plugin_archive_open;
    plugin.plugin.archive.archive_close                     = pak_plugin_archive_close;
    plugin.plugin.archive.archive_get_name                  = pak_plugin_archive_get_name;
    plugin.plugin.archive.archive_get_item_count            = pak_plugin_archive_item_get_count;
    plugin.plugin.archive.archive_foreach_item              = pak_plugin_archive_item_foreach;
    plugin.plugin.archive.archive_locate_item               = pak_plugin_archive_item_locate;
    plugin.plugin.archive.archive_unpack_item_by_filename   = pak_plugin_archive_item_unpack_filename;

    return plugin;
}