
/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// last-rev: 5/11/23

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Stream.hpp>
#include <NewKit/Utils.hpp>

#include <CompilerKit/Compiler.hpp>

namespace hCore
{
    class JsonType final
    {
    public:
        explicit JsonType(SizeT lhsLen, SizeT rhsLen)
            : fKey(lhsLen), fValue(rhsLen)
        {}

        ~JsonType() = default;

        HCORE_COPY_DEFAULT(JsonType);

      private:
        StringView fKey;
        StringView fValue;

      public:
        StringView& AsKey() { return fKey; }
        StringView& AsValue() { return fValue; }

        static JsonType kUndefined;

    };
    
    struct JsonStreamTraits final
    {
        JsonType In(const char* full_array)
        {
            SizeT len = string_length(full_array);

            if (full_array[0] == '\"' &&
                full_array[len - 1] == ',' ||
                full_array[len - 1] == '\"')
            {
                Boolean probe_key = true;

                SizeT key_len = 0;
                SizeT value_len = 0;

                for (SizeT i = 1; i < len; i++)
                {
                    if (full_array[i] == ' ')
                        continue;

                    JsonType type(255, 255);
                    
                    if (probe_key)
                    {
                        type.AsKey().Data()[key_len] = full_array[i];
                        ++key_len;

                        if (full_array[i] == '\"')
                        {
                            probe_key = false;
                            type.AsKey().Data()[key_len] = 0;
                            
                            ++i;
                        }
                    }
                    else
                    {
                        type.AsValue().Data()[value_len] = full_array[i];
                        ++value_len;

                        if (full_array[i] == '\"')
                        {
                            type.AsValue().Data()[value_len] = 0;
                        }     
                    }
                }
                
            }

            return JsonType::kUndefined;
        }

        JsonType Out(JsonType& out)
        {
            return out;
        }
        
    };

    using JsonStream = Stream<JsonStreamTraits, JsonType>;
}