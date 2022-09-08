#pragma once
#include <vector>
#include "../../../maths/vector.hpp"
namespace Kamanri
{
    namespace Renderer
    {
        namespace World
        {
            namespace __
            {
                /**
                 * @brief The Triangle3D class is designed to represent a triangle consist of 3 vertices,
                 * it will be the object of projection transformation and be rendered to screen.
                 *
                 */
                class Triangle3D
                {
                private:
                    // origin data
                    std::vector<Maths::Vector> &_vertices_transform;
                    int _offset;
                    int _v1;
                    int _v2;
                    int _v3;

                    // offset + index
                    int _o_v1, _o_v2, _o_v3;
                    // values
                    double _o_v1_x, _o_v1_y, _o_v2_x, _o_v2_y, _o_v3_x, _o_v3_y;

                    // factors of square, ax + by + cz - 1 = 0
                    double _a;
                    double _b;
                    double _c;

                public:
                    Triangle3D(std::vector<Maths::Vector> &vertices_transform, int offset, int v1, int v2, int v3);
                    void Build();
                    bool IsIn(double x, double y) const;
                    inline double Z(double x, double y) const { return (1 - _a * x - _b * y) / _c; }
                    void PrintTriangle(bool is_print = true) const;
                    bool GetMinMaxWidthHeight(int &min_width, int &min_height, int &max_width, int &max_height) const;
                };
            } // namespace __

        } // namespace Triangle3Ds

    } // namespace Renderer

} // namespace Kamanri