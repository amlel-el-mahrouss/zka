/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#define Z_SOLO 1

#include <ZipKit/Defines.hpp>

#include <CompilerKit/Compiler.hpp>
#include <ZipKit/zlib.hpp>

namespace ZipKit
{
    class ZipStream;

    class ZipStream final
    {
    public:
        explicit ZipStream();
        ~ZipStream() noexcept;

    public:
        HCORE_COPY_DEFAULT(ZipStream);

    public:
        MeFilePtr FlushToFile(const char* name);
        void* Deflate(const char* name);
        void Inflate(const char* name, void* data);

    private:
        void* fSharedData{ nullptr };
        SizeT fSharedSz{ 0 };

    };
}