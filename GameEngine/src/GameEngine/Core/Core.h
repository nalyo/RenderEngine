#pragma once
#include <memory>

#ifdef GE_PLATFORM_WINDOWS
#if GE_DYNAMIC_LINK
	#ifdef GE_BUILD_DLL
		#define GameEngine_API __declspec(dllexport)
	#else 
		#define GameEngine_API __declspec(dllimport)
	#endif
#else
#define GameEngine_API
#endif
#elif defined(GE_PLATFORM_LINUX)
    #define GameEngine_API
#else
    #error GameEngine only supports Windows and Linux!
#endif

#ifdef GE_ENABLE_ASSERTS
    #ifdef GE_PLATFORM_WINDOWS
        #define GE_ASSERT(x, ...) { if(!(x)) {GE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DebugBreak(); } }
        #define GE_CORE_ASSERT(x, ...) { if(!(x)) {GE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DebugBreak(); } }
    #else
        #define GE_ASSERT(x, ...) { if(!(x)) {GE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __builtin_trap(); } }
        #define GE_CORE_ASSERT(x, ...) { if(!(x)) {GE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __builtin_trap(); } }
    #endif
#else
    #define GE_ASSERT(x, ...)
    #define GE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define GE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GameEngine
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}