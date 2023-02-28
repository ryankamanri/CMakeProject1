#pragma once
#include <vector>
#include "kamanri/utils/memory.hpp"
#include "kamanri/renderer/world/__/triangle3d.hpp"
#include "kamanri/maths/all.hpp"
namespace Kamanri
{
    namespace Renderer
    {
        namespace World
        {
			using RGB = unsigned long;
            namespace BlingPhongReflectionModel$
            {

                inline void DivideRGB(RGB color, unsigned int& r, unsigned int& g, unsigned int& b, void (*handle_func)(unsigned int&, RGB))
                {
                    handle_func(r, (color & 0x00ff0000) >> 16);
                    handle_func(g, (color & 0x0000ff00) >> 8);
                    handle_func(b, color & 0x000000ff);
                }


                inline RGB CombineRGB(unsigned int r, unsigned int g, unsigned int b)
                {
                    return ((r < 0xff ? r : 0xff) << 16) | 
                    ((g < 0xff ? g : 0xff) << 8) | 
                    (b < 0xff ? b : 0xff);
                }

                inline RGB RGBAdd(RGB c1, RGB c2)
                {
                    unsigned int r = 0, g = 0, b = 0;
                    DivideRGB(c1, r, g, b, [](unsigned int& y, RGB x){ y += x; });
                    DivideRGB(c2, r, g, b, [](unsigned int& y, RGB x){ y += x; });
                    return CombineRGB(r, g, b);
                }

				inline RGB RGBAdd(RGB c1, RGB c2, RGB c3)
                {
                    unsigned int r = 0, g = 0, b = 0;
                    DivideRGB(c1, r, g, b, [](unsigned int& y, RGB x){ y += x; });
                    DivideRGB(c2, r, g, b, [](unsigned int& y, RGB x){ y += x; });
					DivideRGB(c3, r, g, b, [](unsigned int& y, RGB x){ y += x; });
                    return CombineRGB(r, g, b);
                }

                inline RGB RGBReflect(RGB light, RGB reflect_point)
                {
                    unsigned int r = 0, g = 0, b = 0;
                    DivideRGB(light, r, g, b, [](unsigned int& y, RGB x){ y += x; });
                    DivideRGB(reflect_point, r, g, b, [](unsigned int& y, RGB x){ y *= x; y /= 0xFF; });
                    return CombineRGB(r, g, b);
                }

                inline RGB RGBMul(RGB color, double num)
                {
                    unsigned int r = 0, g = 0, b = 0;
                    DivideRGB(color, r, g, b, [](unsigned int& y, RGB x){ y += x; });
					r = (unsigned int)(r * num);
					g = (unsigned int)(g * num);
					b = (unsigned int)(b * num);
                    return CombineRGB(r, g, b);
                }

                class PointLight
                {

                    public:
                    Maths::Vector location;
					Maths::Vector location_model_view_transformed;
                    double power;
                    RGB color;

                    PointLight(Maths::Vector location, double power, DWORD color):
                        location(location), location_model_view_transformed(location), power(power), color(color) {}

                };

				struct PointLightBufferItem
				{
                    bool is_specular = false;
					bool is_exposed = true;
                    double specular_factor;
					double distance = DBL_MAX;
					PointLightBufferItem() = default;
					PointLightBufferItem(bool is_exposed, double distance):
					is_exposed(is_exposed), distance(distance) {}
				};
            } // namespace BlingPhongReflectionModel$
            
            class BlingPhongReflectionModel
            {
            private:
                /* data */
                std::vector<BlingPhongReflectionModel$::PointLight> _point_lights;
                // std::vector<BlingPhongReflectionModel$::PointLight> _point_lights_model_view_transformed;
                // Note that its size is buffer_size * _point_lights.size()
				Utils::P<BlingPhongReflectionModel$::PointLightBufferItem[]> _lights_buffer;
                size_t _screen_width;
                size_t _screen_height;

                double _specular_min_cos;
				double _ambient_factor;
				double _diffuse_factor;

            public:
				// BlingPhongReflectionModel() = default;
				BlingPhongReflectionModel(std::vector<BlingPhongReflectionModel$::PointLight>&& point_lights, size_t screen_width, size_t screen_height, double specular_min_cos = 0.999, double diffuse_factor = 1 / (Maths::PI * 2), double ambient_factor = 0.1);
                BlingPhongReflectionModel(BlingPhongReflectionModel&& other);
				BlingPhongReflectionModel& operator=(BlingPhongReflectionModel&& other);
                void ModelViewTransform(Maths::SMatrix const& matrix);
				void InitLightBufferPixel(size_t x, size_t y, FrameBuffer& buffer);
                void __BuildPerTrianglePixel(size_t x, size_t y, __::Triangle3D& triangle, FrameBuffer& buffer);
				void WriteToPixel(size_t x, size_t y, FrameBuffer& buffer, DWORD& pixel);

            };
            
            
        } // namespace World
        
    } // namespace Renderer
    
} // namespace Kamanri