//
//  Assert.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 02.06.24.
//
#pragma once

//#include <memory>

// Disable assertions by commenting out this line below
#define SP_ENABLE_ASSERTS

// Custom assert macros with message support
#ifdef SP_ENABLE_ASSERTS
    #include <signal.h>
    #include "Core/Log.hpp"
    #define SP_INTERNAL_ASSERT_IMPL(check, msg) { if(!(check)) { SP_LOG_ERORR("Assertion failed: {0}", msg); raise(SIGTRAP); } }
    
    #define SP_ASSERT(check, msg) SP_INTERNAL_ASSERT_IMPL(check, msg)
#else
    
#endif
