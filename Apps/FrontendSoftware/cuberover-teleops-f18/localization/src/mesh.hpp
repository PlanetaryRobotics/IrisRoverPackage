#ifndef MESH_HPP_
#define MESH_HPP_

#include <iostream>
#include <opencv2/core/core.hpp>

class Triangle {
 public:
  explicit Triangle(const cv::Point3f& V0, const cv::Point3f& V1, const cv::Point3f& V2);
  virtual ~Triangle();

  cv::Point3f getV0() const { return v0_; }
  cv::Point3f getV1() const { return v1_; }
  cv::Point3f getV2() const { return v2_; }

 private:
  /** The three vertices that defines the triangle */
  cv::Point3f v0_, v1_, v2_;
};

class Ray {
 public:
  explicit Ray(const cv::Point3f& P0, const cv::Point3f& P1);
  virtual ~Ray();

  cv::Point3f getP0() { return p0_; }
  cv::Point3f getP1() { return p1_; }

 private:
  /** The two points that defines the ray */
  cv::Point3f p0_, p1_;
};

class Mesh {
 public:
  Mesh();
  virtual ~Mesh();
  std::vector<std::vector<int> > getTrianglesList() const { return list_triangles_; }
  cv::Point3f getVertex(int pos) const { return list_vertex_[pos]; }
  int getNumVertices() const { return num_vertices_; }
  void load(const std::string& path_file);

 private:
  /** The current number of vertices in the mesh */
  int num_vertices_;
  /** The current number of triangles in the mesh */
  int num_triangles_;
  /* The list of triangles of the mesh */
  std::vector<cv::Point3f> list_vertex_;
  /* The list of triangles of the mesh */
  std::vector<std::vector<int> > list_triangles_;
};

#endif /* OBJECTMESH_HPP_ */
