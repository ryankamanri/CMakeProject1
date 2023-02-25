#include <cfloat>
#include "kamanri/utils/string.hpp"
#include "kamanri/utils/logs.hpp"
#include "kamanri/renderer/world/__/buffers.hpp"
#include "cuda_dll/exports/memory_operations.hpp"

using namespace Kamanri::Renderer::World;
using namespace Kamanri::Renderer::World::__;
using namespace Kamanri::Utils;

namespace Kamanri
{
	namespace Renderer
	{
		namespace World
		{
			namespace __
			{
				namespace __Buffers
				{
					constexpr const char* LOG_NAME = STR(Kamanri::Renderer::World::__::Buffers);

					dll cuda_dll;
					func_type(CUDAMalloc) cuda_malloc;
					func_type(CUDAFree) cuda_free;
					func_type(TransmitToCUDA) transmit_to_cuda;
					func_type(TransmitFromCUDA) transmit_from_cuda;

					void ImportFunctions()
					{
						load_dll(cuda_dll, cuda_dll, LOG_NAME);
						import_func(CUDAMalloc, cuda_dll, cuda_malloc, LOG_NAME);
						import_func(CUDAFree, cuda_dll, cuda_free, LOG_NAME);
						import_func(TransmitToCUDA, cuda_dll, transmit_to_cuda, LOG_NAME);
						import_func(TransmitFromCUDA, cuda_dll, transmit_from_cuda, LOG_NAME);
					}
				} // namespace __Buffers
				
			} // namespace __
			
		} // namespace World
		
	} // namespace Renderer
	
} // namespace Kamanri

#define Loc(x, y) ((_height - (y + 1)) * _height + x)

Buffers::~Buffers()
{
	Log::Debug(__Buffers::LOG_NAME, "clean the buffers");
	__Buffers::cuda_free(_cuda_buffers);
	__Buffers::cuda_free(_cuda_bitmap_buffer);
}
void Buffers::Init(size_t width, size_t height)
{   
	_width = width;
	_height = height;
	_buffers = NewArray<FrameBuffer>(width * height);
	_bitmap_buffer = NewArray<DWORD>(width * height);

	__Buffers::ImportFunctions();

	auto buffers_size = width * height * sizeof(FrameBuffer);
	__Buffers::cuda_malloc(&(void*)_cuda_buffers, buffers_size);
	__Buffers::transmit_to_cuda(_buffers.get(), _cuda_buffers, buffers_size);
	
	auto bitmap_buffer_size = width * height * sizeof(DWORD);
	__Buffers::cuda_malloc(&(void*)_cuda_bitmap_buffer, bitmap_buffer_size);
	__Buffers::transmit_to_cuda(_bitmap_buffer.get(), _cuda_bitmap_buffer, bitmap_buffer_size);

}

Buffers& Buffers::operator=(Buffers&& other)
{
	_width = other._width;
	_height = other._height;
	_buffers = std::move(other._buffers);
	_bitmap_buffer = std::move(other._bitmap_buffer);
	return *this;
}

void Buffers::CleanAllBuffers() const
{
	for(size_t i = 0; i < _width; i++)
	{
		for(size_t j = 0; j < _height; j++)
		{
			_buffers[Loc(i, j)].z = -DBL_MAX;
		}
	}
	ZeroMemory(_bitmap_buffer.get(), _width * _height * sizeof(DWORD));
}


FrameBuffer& Buffers::GetFrame(size_t x, size_t y)
{
	if(x < 0 || y < 0 || x >= _width || y >= _height)
	{
		Log::Error(__Buffers::LOG_NAME, "Invalid Index (%d, %d), return the 0 index content", x, y);
		PRINT_LOCATION;
		return _buffers[0];
	}
	return _buffers[Loc(x, y)];
	
}

// #define Loc(x, y, width, height) ()

DWORD& Buffers::GetBitmapBuffer(size_t x, size_t y)
{
	if(x < 0 || y < 0 || x >= _width || y >= _height)
	{
		Log::Error(__Buffers::LOG_NAME, "Invalid Index (%d, %d), return the 0 index content", x, y);
		PRINT_LOCATION;
		return _bitmap_buffer[0];
	}
	return _bitmap_buffer[Loc(x, y)]; // (x, y) -> (x, _height - y)
}