/***********************************************************************************
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
#ifndef _VFS_HPP_
#define _VFS_HPP_

#include "vfs.h"
#include "file.h"


class VFS
{
    VFS(){}
    VFS(const VFS&);
    VFS& operator=(const VFS&);

public:
    ~VFS()
	{
		Destroy();
	}

    static VFS& GetInstance(){
		static VFS Instance;
        return Instance;
    }

public:

    bool Create(const char*workpath)
    {
        if(VFS_TRUE == vfs_create(workpath))
            return true;
        else
            return false;
    }

    void Destroy()
    {
        vfs_destroy();
    }

    bool AddPath(const char* path )
    {
        if( VFS_TRUE == vfs_add_pak(path) )
            return true;
        else
            return false;
    }

    bool FileExists(const char* file )
    {
        if( !file)
            return false;

        int count = vfs_get_pak_count();
        for( int i = 0; i<count; ++i )
        {
            pak* _pak = vfs_get_pak_index(i);
            if( _pak && pak_item_locate(_pak,file) >= 0  )
                return true;
        }

        return false;
    }
};

#define sglVFS VFS::GetInstance()

#endif//_VFS_HPP_