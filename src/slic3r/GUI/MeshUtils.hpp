#ifndef slic3r_MeshUtils_hpp_
#define slic3r_MeshUtils_hpp_

#include "libslic3r/Point.hpp"
#include "libslic3r/Geometry.hpp"


#include <cfloat>

namespace Slic3r {

class TriangleMesh;
class TriangleMeshSlicer;

namespace GUI {



class ClippingPlane
{
    double m_data[4];

public:
    ClippingPlane()
    {
        m_data[0] = 0.0;
        m_data[1] = 0.0;
        m_data[2] = 1.0;
        m_data[3] = 0.0;
    }

    ClippingPlane(const Vec3d& direction, double offset)
    {
        Vec3d norm_dir = direction.normalized();
        m_data[0] = norm_dir(0);
        m_data[1] = norm_dir(1);
        m_data[2] = norm_dir(2);
        m_data[3] = offset;
    }

    bool operator==(const ClippingPlane& cp) const {
        return m_data[0]==cp.m_data[0] && m_data[1]==cp.m_data[1] && m_data[2]==cp.m_data[2] && m_data[3]==cp.m_data[3];
    }
    bool operator!=(const ClippingPlane& cp) const { return ! (*this==cp); }

    double distance(const Vec3d& pt) const {
        assert(is_approx(get_normal().norm(), 1.));
        return (-get_normal().dot(pt) + m_data[3]);
    }

    void set_normal(const Vec3d& normal) { for (size_t i=0; i<3; ++i) m_data[i] = normal(i); }
    void set_offset(double offset) { m_data[3] = offset; }
    Vec3d get_normal() const { return Vec3d(m_data[0], m_data[1], m_data[2]); }
    bool is_active() const { return m_data[3] != DBL_MAX; }
    static ClippingPlane ClipsNothing() { return ClippingPlane(Vec3d(0., 0., 1.), DBL_MAX); }
    const double* get_data() const { return m_data; }

    // Serialization through cereal library
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( m_data[0], m_data[1], m_data[2], m_data[3] );
    }
};



class MeshClipper {
public:
    void set_plane(const ClippingPlane& plane);
    void set_mesh(const TriangleMesh& mesh);
    void set_transformation(const Geometry::Transformation& trafo);

    const std::vector<Vec3f>& get_triangles();

private:
    void recalculate_triangles();

    Geometry::Transformation m_trafo;
    const TriangleMesh* m_mesh = nullptr;
    ClippingPlane m_plane;
    std::vector<Vec2f> m_triangles2d;
    std::vector<Vec3f> m_triangles3d;
    bool m_triangles_valid = false;
    std::unique_ptr<TriangleMeshSlicer> m_tms;
};


    
} // namespace GUI
} // namespace Slic3r


#endif // slic3r_MeshUtils_hpp_