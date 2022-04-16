#pragma once

#include "PoolAllocator.h"

namespace ECS
{
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

		STLPoolAlloc(STLPoolAlloc& alloc) noexcept;

		/* STL required copy constructor */
		template<typename OtherType>
		STLPoolAlloc(const STLPoolAlloc<OtherType>&) noexcept {}

	private:
		STLPoolAlloc* CopiedAllocator;
	};

	template<typename Type>
	STLPoolAlloc<Type>::STLPoolAlloc(STLPoolAlloc& alloc) noexcept
		: CopiedAllocator{ &alloc }
	{}
}