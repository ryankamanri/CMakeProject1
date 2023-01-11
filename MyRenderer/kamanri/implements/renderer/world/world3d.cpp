#include "kamanri/renderer/world/world3d.hpp"
#include "kamanri/utils/string.hpp"


using namespace Kamanri::Renderer::World;
using namespace Kamanri::Maths;
using namespace Kamanri::Utils;

namespace Kamanri
{
    namespace Renderer
    {
        namespace World
        {
            namespace __World3D
            {
                constexpr const char* LOG_NAME = STR(Kamanri::Renderer::World::World3D);
            } // namespace __World3D
            
        } // namespace World
        
    } // namespace Renderer
    
} // namespace Kamanri

World3D::World3D(): _camera(Camera()) {}

World3D::World3D(Camera&& camera): _camera(std::move(camera))
{
    _camera.SetVertices(_vertices, _vertices_transform);
    
    _buffers.Init(_camera.ScreenWidth(), _camera.ScreenHeight());
}

World3D& World3D::operator=(World3D && other)
{
    _vertices = std::move(other._vertices);
    _vertices_transform = std::move(other._vertices_transform);
    _camera = std::move(other._camera);
    _environment = std::move(other._environment);
    _buffers = std::move(other._buffers);
    // Move the reference of vertices of camera
    _camera.SetVertices(_vertices, _vertices_transform);
    return *this;
}

Result<Object> World3D::AddObjModel(ObjModel const &model, bool is_print)
{
    auto dot_offset = (int)_vertices.size();

    for(auto i = 0; i < model.GetVertexSize(); i++)
    {
        auto vertex = TRY_FOR_TYPE(Object, model.GetVertex(i));
        Vector vector = {vertex[0], vertex[1], vertex[2], 1};
        _vertices.push_back(vector);
        _vertices_transform.push_back(vector);
    }


    for(auto i = 0; i < model.GetFaceSize(); i++)
    {
        auto face = TRY_FOR_TYPE(Object, model.GetFace(i));
        if(face.vertex_indexes.size() > 4)
        {
            auto message = "Can not handle `face.vertex_indexes() > 4`";
            Log::Error(__World3D::LOG_NAME, message);
            return RESULT_EXCEPTION(Object, World3D$::CODE_UNHANDLED_EXCEPTION, message);
        }
        if(face.vertex_indexes.size() == 4)
        {
            auto triangle = __::Triangle3D(dot_offset, face.vertex_indexes[0] - 1, face.vertex_indexes[3] - 1, face.vertex_indexes[2] - 1);
            this->_environment.triangles.push_back(triangle);
        }
        auto triangle2 = __::Triangle3D(dot_offset, face.vertex_indexes[0] - 1, face.vertex_indexes[1] - 1, face.vertex_indexes[2] - 1);
        
        _environment.triangles.push_back(triangle2);
    }

    // do check
    for(auto i = 0; i < _environment.triangles.size(); i++)
    {
        _environment.triangles[i].PrintTriangle(is_print);
    }

    return Result<Object>(Object(_vertices, dot_offset, (int)model.GetVertexSize()));
}


World3D&& World3D::AddObjModel(ObjModel const& model, Maths::SMatrix const& transform_matrix, bool is_print)
{
    auto res = AddObjModel(model, is_print);
    if(res.IsException())
    {
        res.Print();
    }
    res.Data().Transform(transform_matrix);
    return std::move(*this);
}

DefaultResult World3D::Build(bool is_print)
{
    Log::Info(__World3D::LOG_NAME, "Start to build the world...");
    _buffers.CleanZBuffer();
    for(auto i = _environment.triangles.begin(); i != _environment.triangles.end(); i++)
    {
        i->Build(_vertices_transform);
        i->PrintTriangle(is_print);
        _buffers.WriteToZBufferFrom(*i);
        // areal coordinate test
        i->PrintTriangle(is_print);
        if(i->IsIn(300, 500))
        {
            auto ac_v = TRY_FOR_DEFAULT(i->ArealCoordinates(300, 500, true));
            PrintLn("The areal coordinate vector:");
            ac_v.PrintVector(is_print);
        }

    }

    return DEFAULT_RESULT;
}

double World3D::Depth(int x, int y)
{
    
    x = x % _buffers.Height();
    y = y % _buffers.Width();
    
    return _buffers.Get(x, y).z;
}
