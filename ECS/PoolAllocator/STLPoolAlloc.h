#pragma once

#include <iostream>
#include "PoolAllocator.h"

namespace ECS
{
	/* Based on: */
	/* https://github.com/moya-lang/Allocator/blob/master/Allocator.h */

	template<typename Type>
	class STLPoolAlloc final : private PoolAllocator<Type>
	{
	public:
		/* STL using directives */

		using value_type = Type;
		using reference = Type&;
		using const_reference = const Type&;
		using pointer = Type*;
		using const_pointer = const Type*;
		using difference_type = std::ptrdiff_t;
		using size_type = size_t;

		template<typename OtherType>
		struct rebind
		{
			using other = STLPoolAlloc<OtherType>;
		};

		STLPoolAlloc() = default;
		virtual ~STLPoolAlloc() = default;

		STLPoolAlloc(const STLPoolAlloc&) noexcept = default;
		STLPoolAlloc(STLPoolAlloc&& ) noexcept = default;
		STLPoolAlloc& operator=(const STLPoolAlloc&) noexcept = default;
		STLPoolAlloc& operator=(STLPoolAlloc&&) noexcept = default;

		/* STL required copy constructor */
		template<typename OtherType>
		STLPoolAlloc(const STLPoolAlloc<OtherType>&) noexcept {}

		pointer allocate(size_type elementsToAllocate);

		/* unreferenced size_type for STL and noexcept for STL */
		void deallocate(pointer p, size_type = 0) noexcept;

		void construct(pointer p, const_reference value);
		void destroy(pointer p);

		pointer address(reference r) const;
		const_pointer address(const_reference r) const;
	};

	template<typename Type>
	typename STLPoolAlloc<Type>::pointer STLPoolAlloc<Type>::allocate(size_type elementsToAllocate)
	{
		return PoolAllocator<Type>::allocate(elementsToAllocate);
	}

	template<typename Type>
	void STLPoolAlloc<Type>::deallocate(pointer p, size_type c) noexcept
	{
		return PoolAllocator<Type>::deallocate(p);
	}

	template<typename Type>
	void STLPoolAlloc<Type>::construct(pointer p, const_reference value)
	{
		new (p) Type(value);
	}

	template<typename Type>
	void STLPoolAlloc<Type>::destroy(pointer p)
	{
		if constexpr (!std::is_trivially_destructible_v<Type>)
		{
			p->~Type();
		}
	}
	
	template<typename Type>
	STLPoolAlloc<Type>::pointer STLPoolAlloc<Type>::address(reference r) const
	{
		return &r;
	}

	template<typename Type>
	STLPoolAlloc<Type>::const_pointer STLPoolAlloc<Type>::address(const_reference r) const
	{
		return &r;
	}

	template<typename Type, typename OtherType>
	bool operator==(const STLPoolAlloc<Type>&, const STLPoolAlloc<OtherType>&) { return true; }

	template<typename Type, typename OtherType>
	bool operator!=(const STLPoolAlloc<Type>&, const STLPoolAlloc<OtherType>&) { return false; }
}