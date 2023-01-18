#pragma once
#include "kamanri/utils/memory.hpp"
#include "kamanri/renderer/world/frame_buffer.hpp"
#include "triangle3d.hpp"


namespace Kamanri
{
    namespace Renderer
    {
        namespace World
        {
            namespace __
            {
                class Buffers
                {
                private:
                    unsigned int _width;
                    unsigned int _height;
                    Utils::P<FrameBuffer[]> _buffers;

                public:
                    void Init(unsigned int width, unsigned int height);
                    Buffers& operator=(Buffers&& other);
                    void CleanZBuffer() const;
                    void WriteToZBufferFrom(Triangle3D const &t, bool is_print = false);
                    inline int Width() const { return _width; }
                    inline int Height() const { return _height; }
                    FrameBuffer& Get(int width, int height);
                };

            } // namespace __

        } // namespace World

    } // namespace Renderer

} // namespace Kamanri