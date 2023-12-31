#include "gcf.h"
#include "Ray.h"
#include "Transform.h"

const Transform Transform::Identity(new Matrix4x4());

Transform::Transform():
    m_mdir(nullptr), m_minv(nullptr)
{

}

Transform::Transform(
    double t00, double t01, double t02, double t03,
    double t10, double t11, double t12, double t13,
    double t20, double t21, double t22, double t23,
    double t30, double t31, double t32, double t33
)
{
    m_mdir = std::make_shared<Matrix4x4>(
        t00, t01, t02, t03,
        t10, t11, t12, t13,
        t20, t21, t22, t23,
        t30, t31, t32, t33
    );
    m_minv = m_mdir->inversed();
}

Transform::Transform(double m[4][4])
{
    m_mdir = std::make_shared<Matrix4x4>(
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3]
    );
    m_minv = m_mdir->inversed();
}

Transform::Transform(Matrix4x4* m):
    m_mdir(m),
    m_minv(m->inversed())
{

}

Transform::Transform(const std::shared_ptr<Matrix4x4>& mdir):
    m_mdir(mdir),
    m_minv(mdir->inversed())
{

}

Transform::Transform(const std::shared_ptr<Matrix4x4>& mdir, const std::shared_ptr<Matrix4x4>& minv):
    m_mdir(mdir),
    m_minv(minv)
{

}

vec3d Transform::getScales() const
{
    // https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati/417813
    // polar decomposition

    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];

    return vec3d(
        vec3d(t0[0], t1[0], t2[0]).norm(),
        vec3d(t0[1], t1[1], t2[1]).norm(),
        vec3d(t0[2], t1[2], t2[2]).norm()
    );
}

bool Transform::SwapsHandedness() const //?
{
    double det =
        m_mdir->m[0][0]*(m_mdir->m[1][1]*m_mdir->m[2][2] - m_mdir->m[1][2]*m_mdir->m[2][1]) -
        m_mdir->m[0][1]*(m_mdir->m[1][0]*m_mdir->m[2][2] - m_mdir->m[1][2]*m_mdir->m[2][0]) +
        m_mdir->m[0][2]*(m_mdir->m[1][0]*m_mdir->m[2][1] - m_mdir->m[1][1]*m_mdir->m[2][0]);
    return det < 0.;
}

Transform Transform::operator*(const Transform& t) const
{
    return Transform(
        multiply(*m_mdir, *t.m_mdir),
        multiply(*t.m_minv, *m_minv)
    );
}

vec3d Transform::transformPoint(const vec3d& p) const
{
    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];
    return vec3d(
        t0[0]*p.x + t0[1]*p.y + t0[2]*p.z + t0[3],
        t1[0]*p.x + t1[1]*p.y + t1[2]*p.z + t1[3],
        t2[0]*p.x + t2[1]*p.y + t2[2]*p.z + t2[3]
    );
}

vec3d Transform::transformVector(const vec3d& v) const
{
    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];
    return vec3d(
        t0[0]*v.x + t0[1]*v.y + t0[2]*v.z,
        t1[0]*v.x + t1[1]*v.y + t1[2]*v.z,
        t2[0]*v.x + t2[1]*v.y + t2[2]*v.z
    );
}

//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals
vec3d Transform::transformNormal(const vec3d& n) const
{
    const double* t0 = m_minv->m[0];
    const double* t1 = m_minv->m[1];
    const double* t2 = m_minv->m[2];
    return vec3d(
        t0[0]*n.x + t1[0]*n.y + t2[0]*n.z,
        t0[1]*n.x + t1[1]*n.y + t2[1]*n.z,
        t0[2]*n.x + t1[2]*n.y + t2[2]*n.z
    );
}

vec3d Transform::transformInverseNormal(const vec3d& n) const
{
    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];
    return vec3d(
        t0[0]*n.x + t1[0]*n.y + t2[0]*n.z,
        t0[1]*n.x + t1[1]*n.y + t2[1]*n.z,
        t0[2]*n.x + t1[2]*n.y + t2[2]*n.z
    );
}

Ray Transform::transformDirect(const Ray& r) const
{
    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];

    const vec3d& p = r.origin;
    vec3d o(
        t0[0]*p.x + t0[1]*p.y + t0[2]*p.z + t0[3],
        t1[0]*p.x + t1[1]*p.y + t1[2]*p.z + t1[3],
        t2[0]*p.x + t2[1]*p.y + t2[2]*p.z + t2[3]
    );

    const vec3d& v = r.direction();
    vec3d d(
        t0[0]*v.x + t0[1]*v.y + t0[2]*v.z,
        t1[0]*v.x + t1[1]*v.y + t1[2]*v.z,
        t2[0]*v.x + t2[1]*v.y + t2[2]*v.z
    );
//    const double* ti0 = m_minv->m[0];
//    const double* ti1 = m_minv->m[1];
//    const double* ti2 = m_minv->m[2];
//    Vector3D d(
//        ti0[0]*v.x + ti1[0]*v.y + ti2[0]*v.z,
//        ti0[1]*v.x + ti1[1]*v.y + ti2[1]*v.z,
//        ti0[2]*v.x + ti1[2]*v.y + ti2[2]*v.z
//    );

    return Ray(o, d, r.tMin, r.tMax);
}

Ray Transform::transformInverse(const Ray& r) const
{
    const double* t0 = m_minv->m[0];
    const double* t1 = m_minv->m[1];
    const double* t2 = m_minv->m[2];

    const vec3d& p = r.origin;
    vec3d o(
        t0[0]*p.x + t0[1]*p.y + t0[2]*p.z + t0[3],
        t1[0]*p.x + t1[1]*p.y + t1[2]*p.z + t1[3],
        t2[0]*p.x + t2[1]*p.y + t2[2]*p.z + t2[3]
    );

    const vec3d& v = r.direction();
    vec3d d(
        t0[0]*v.x + t0[1]*v.y + t0[2]*v.z,
        t1[0]*v.x + t1[1]*v.y + t1[2]*v.z,
        t2[0]*v.x + t2[1]*v.y + t2[2]*v.z
    );
//    const double* ti0 = m_mdir->m[0];
//    const double* ti1 = m_mdir->m[1];
//    const double* ti2 = m_mdir->m[2];
//    Vector3D d(
//        ti0[0]*v.x + ti1[0]*v.y + ti2[0]*v.z,
//        ti0[1]*v.x + ti1[1]*v.y + ti2[1]*v.z,
//        ti0[2]*v.x + ti1[2]*v.y + ti2[2]*v.z
//    );

    return Ray(o, d, r.tMin, r.tMax);
}


vec3d Transform::operator()(const vec3d& v) const
{
    return vec3d(
        m_mdir->m[0][0]*v.x + m_mdir->m[0][1]*v.y + m_mdir->m[0][2]*v.z,
        m_mdir->m[1][0]*v.x + m_mdir->m[1][1]*v.y + m_mdir->m[1][2]*v.z,
        m_mdir->m[2][0]*v.x + m_mdir->m[2][1]*v.y + m_mdir->m[2][2]*v.z
    );
}

void Transform::operator()(const vec3d& v, vec3d& ans) const
{
    ans.x = m_mdir->m[0][0]*v.x + m_mdir->m[0][1]*v.y + m_mdir->m[0][2]*v.z;
    ans.y = m_mdir->m[1][0]*v.x + m_mdir->m[1][1]*v.y + m_mdir->m[1][2]*v.z;
    ans.z = m_mdir->m[2][0]*v.x + m_mdir->m[2][1]*v.y + m_mdir->m[2][2]*v.z;
}

Ray Transform::operator()(const Ray& r) const
{
    Ray ans;
    ans.origin = transformPoint(r.origin);
    vec3d d = transformVector(r.direction());
    ans.setDirection(d);
    ans.tMin = r.tMin;
    ans.tMax = r.tMax;
    return ans;
}

void Transform::operator()(const Ray& r, Ray& ans) const
{
    ans.origin = transformPoint(r.origin);
    vec3d d = transformVector(r.direction());
    ans.setDirection(d);
    ans.tMin = r.tMin;
    ans.tMax = r.tMax;
}

bool Transform::operator==(const Transform& t) const
{
    if (this == &t) return true;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (!gcf::equals(m_mdir->m[i][j], t.m_mdir->m[i][j]))
                return false;

    return true;
}

vec3d Transform::multVecMatrix(const vec3d& v) const
{
    vec3d ans;

    const double* t0 = m_mdir->m[0];
    const double* t1 = m_mdir->m[1];
    const double* t2 = m_mdir->m[2];
    const double* t3 = m_mdir->m[3];

    double w = t3[0]*v[0] + t3[1]*v[1] + t3[2]*v[2] + t3[3];

    if (w != 0.) {
        ans[0] = (t0[0]*v[0] + t0[1]*v[1] + t0[2]*v[2] + t0[3])/w;
        ans[1] = (t1[0]*v[0] + t1[1]*v[1] + t1[2]*v[2] + t1[3])/w;
        ans[2] = (t2[0]*v[0] + t2[1]*v[1] + t2[2]*v[2] + t2[3])/w;
    }

    return ans;
}

vec3d Transform::multDirMatrix(const vec3d& src) const
{
    vec3d dst;
    // Checks if the "this" matrix is equal to the identity matrix.  See
    // also code comments at the start of SbMatrix::multRight().
    //if (SbMatrixP::isIdentity(this->matrix)) { dst = src; return dst; }


    const double* t0 = (m_mdir->m)[0];
    const double* t1 = (m_mdir->m)[1];
    const double* t2 = (m_mdir->m)[2];
    // Copy the src vector, just in case src and dst is the same vector.
    dst[0] = src[0]*t0[0] + src[1]*t0[1] + src[2]*t0[2];
    dst[1] = src[0]*t1[0] + src[1]*t1[1] + src[2]*t1[2];
    dst[2] = src[0]*t2[0] + src[1]*t2[1] + src[2]*t2[2];

    return dst;
}

Transform Transform::translate(double x, double y, double z)
{
    auto mdir = std::make_shared<Matrix4x4>(
        1., 0., 0., x,
        0., 1., 0., y,
        0., 0., 1., z,
        0., 0., 0., 1.
    );

    auto minv = std::make_shared<Matrix4x4>(
        1., 0., 0., -x,
        0., 1., 0., -y,
        0., 0., 1., -z,
        0., 0., 0., 1.
    );

    return Transform(mdir, minv);
}

Transform Transform::scale(double sx, double sy, double sz)
{
    auto mdir = std::make_shared<Matrix4x4>(
        sx, 0., 0., 0.,
        0., sy, 0., 0.,
        0., 0., sz, 0.,
        0., 0., 0., 1.
    );

    auto minv = std::make_shared<Matrix4x4>(
        1./sx, 0., 0., 0.,
        0., 1./sy, 0., 0.,
        0., 0., 1./sz, 0.,
        0., 0., 0., 1.
    );

    return Transform(mdir, minv);
}

// angle in radians
Transform Transform::rotateX(double angle)
{
    double c = cos(angle);
    double s = sin(angle);

    auto mdir = std::make_shared<Matrix4x4>(
        1., 0., 0., 0.,
        0., c, -s, 0.,
        0., s, c, 0.,
        0., 0., 0., 1.
    );

    return Transform(mdir, mdir->transposed());
}

Transform Transform::rotateY(double angle)
{
    double c = cos(angle);
    double s = sin(angle);

    auto mdir = std::make_shared<Matrix4x4>(
        c, 0., s, 0.,
        0., 1., 0., 0.,
        -s, 0., c, 0.,
        0., 0., 0., 1.
    );

    return Transform(mdir, mdir->transposed());
}

Transform Transform::rotateZ(double angle)
{
    double c = cos(angle);
    double s = sin(angle);

    auto mdir = std::make_shared<Matrix4x4>(
        c, -s, 0., 0.,
        s,  c, 0., 0.,
        0., 0., 1., 0.,
        0., 0., 0., 1.
    );

    return Transform(mdir, mdir->transposed());
}

Transform Transform::rotate(double angle, const vec3d& axis)
{
    vec3d a = axis.normalized();
    double s = sin(angle);
    double c = cos(angle);
    double d = 1. - c;
    double m[4][4];

    m[0][0] = a.x*a.x*d + c;
    m[0][1] = a.x*a.y*d - a.z*s;
    m[0][2] = a.x*a.z*d + a.y*s;
    m[0][3] = 0.;

    m[1][0] = a.y*a.x*d + a.z*s;
    m[1][1] = a.y*a.y*d + c;
    m[1][2] = a.y*a.z*d - a.x*s;
    m[1][3] = 0.;

    m[2][0] = a.z*a.x*d - a.y*s;
    m[2][1] = a.z*a.y*d + a.x*s;
    m[2][2] = a.z*a.z*d + c;
    m[2][3] = 0.;

    m[3][0] = 0.;
    m[3][1] = 0.;
    m[3][2] = 0.;
    m[3][3] = 1.;

    auto mdir = std::make_shared<Matrix4x4>(m);
    return Transform(mdir, mdir->transposed());
}

Transform Transform::LookAt(const vec3d& pos, const vec3d& look, const vec3d& up)
{
    double m[4][4];
    m[0][3] = pos.x;
    m[1][3] = pos.y;
    m[2][3] = pos.z;
    m[3][3] = 1.;

    vec3d dir = (look - pos).normalized();
    vec3d right = cross(dir, up).normalized();
    vec3d newUp = cross(right, dir);

    m[0][0] = right.x;
    m[1][0] = right.y;
    m[2][0] = right.z;
    m[3][0] = 0.;

    m[0][1] = dir.x;
    m[1][1] = dir.y;
    m[2][1] = dir.z;
    m[3][1] = 0.;

    m[0][2] = newUp.x;
    m[1][2] = newUp.y;
    m[2][2] = newUp.z;
    m[3][2] = 0.;

    auto camToWorld = std::make_shared<Matrix4x4>(m);
    return Transform(camToWorld->inversed(), camToWorld);
}

std::ostream& operator<<(std::ostream& os, const Transform& t)
{
    os << *t.getMatrix();
    return os;
}
