#pragma once

namespace ECS
{
	class Allocator final
	{
	public:
		Allocator();
		explicit Allocator(const size_t totalSize);

		~Allocator();

	private:
		size_t RoundToNextPowerOfTwo(size_t v) const
		{
			v--;
			v |= v >> 1u;
			v |= v >> 2u;
			v |= v >> 4u;
			v |= v >> 8u;
			v |= v >> 16u;
			v |= v >> 32u;
			v++;
		}

		void* pBuffer;
		size_t StackPointer;
	};
}