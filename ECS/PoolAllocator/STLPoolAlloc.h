#pragma once

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
		~STLPoolAlloc();

		STLPoolAlloc(STLPoolAlloc& alloc) noexcept;

		/* STL required copy constructor */
		template<typename OtherType>
		STLPoolAlloc(const STLPoolAlloc<OtherType>&) noexcept;

		pointer allocate(size_type elementsToAllocate);

		/* unreferenced size_type for STL and noexcept for STL */
		void deallocate(pointer p, size_type = 0) noexcept;

		void construct(pointer p, const_reference value);
		void destroy(pointer p);

	private:
		STLPoolAlloc* CopiedAllocator{};
		std::allocator<Type>* RebindAllocator{};
	};

	template<typename Type>
	STLPoolAlloc<Type>::~STLPoolAlloc()
	{
		if (RebindAllocator)
		{
			delete RebindAllocator;
		}
	}

	template<typename Type>
	STLPoolAlloc<Type>::STLPoolAlloc(STLPoolAlloc& alloc) noexcept
		: CopiedAllocator{ &alloc }
	{}

	template<typename Type>
	template<typename OtherType>
	STLPoolAlloc<Type>::STLPoolAlloc(const STLPoolAlloc<OtherType>&) noexcept
	{
		if (!std::is_same_v<Type, OtherType>)
		{
			RebindAllocator = new std::allocator<Type>();
		}
	}

	template<typename Type>
	typename STLPoolAlloc<Type>::pointer STLPoolAlloc<Type>::allocate(size_type elementsToAllocate)
	{
		if (CopiedAllocator)
		{
			return CopiedAllocator->allocate(elementsToAllocate);
		}

		if (RebindAllocator)
		{
			return RebindAllocator->allocate(elementsToAllocate);
		}

		return PoolAllocator<Type>::allocate(elementsToAllocate);
	}

	template<typename Type>
	void STLPoolAlloc<Type>::deallocate(pointer p, size_type c) noexcept
	{
		if (CopiedAllocator)
		{
			return CopiedAllocator->deallocate(p);
		}

		if (RebindAllocator)
		{
			return RebindAllocator->deallocate(p, c);
		}

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
		p->~Type();
	}
}