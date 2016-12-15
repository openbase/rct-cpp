/*
 */

#include "Transform.h"
namespace rct {
  
Transform::Transform() {
	transform = new Eigen::Affine3d();
  
	frameParent = "uninitialized";
	frameChild = "uninitialized";
	authority = "uninitialized";

	time = boost::posix_time::min_date_time;
	
	type = DYNAMIC;
}

Transform::~Transform() {
	delete transform;
}

Transform::Transform(const Transform & other) {
	transform = new Eigen::Affine3d();
	*this = other;  
}

Transform::Transform(const Eigen::Affine3d &transform, const std::string &frameParent,
			const std::string &frameChild, const boost::posix_time::ptime &time,const rct::TransformType &type) {
	this->transform = new Eigen::Affine3d();
	*(this->transform) = transform;
  
	this->frameParent = frameParent;
	this->frameChild = frameChild;
	authority = "uninitialized";

	this->time = time;
	
	this->type = type;
}

Transform & Transform::operator=(const Transform & rhs) {
	// Only do assignment if RHS is a different object from this.
	if (this != &rhs) {
		*transform = *rhs.transform;
		frameParent = rhs.frameParent;
		frameChild = rhs.frameChild;
		authority = rhs.authority;
		time = rhs.time;
		type = rhs.type;
	}
	return *this;
}

const std::string& Transform::getFrameChild() const {
      return frameChild;
}

void Transform::setFrameChild(const std::string& frameChild) {
      this->frameChild = frameChild;
}

const std::string& Transform::getFrameParent() const {
      return frameParent;
}

void Transform::setFrameParent(const std::string& frameParent) {
      this->frameParent = frameParent;
}

const boost::posix_time::ptime& Transform::getTime() const {
      return time;
}

void Transform::setTime(const boost::posix_time::ptime& time) {
      this->time = time;
}

const std::string& Transform::getAuthority() const {
      return authority;
}

void Transform::setAuthority(const std::string &authority) {
      this->authority = authority;
}

const rct::TransformType& Transform::getTransformType() const {
      return type;
}
void Transform::setTransformType(const rct::TransformType& tType) {
      this->type = tType;
}

const Eigen::Affine3d& Transform::getTransform() const {
      return *transform;
}

void Transform::setTransform(const Eigen::Affine3d& transform) {
      *(this->transform) = transform;
}

Eigen::Vector3d Transform::getTranslation() const {
      return transform->translation();
}

Eigen::Quaterniond Transform::getRotationQuat() const {
      Eigen::Quaterniond quat(transform->rotation().matrix());
      return quat;
}

Eigen::Vector3d Transform::getRotationYPR() const {

      Eigen::Matrix3d mat = transform->rotation().matrix();

// this code is taken from buttel btMatrix3x3 getEulerYPR().
// http://bulletphysics.org/Bullet/BulletFull/btMatrix3x3_8h_source.html
      // first use the normal calculus
      double yawOut = atan2(mat(1,0), mat(0,0));
      double pitchOut = asin(-mat(2,0));
      double rollOut = atan2(mat(2,1), mat(2,2));

      // on pitch = +/-HalfPI
      if (abs(pitchOut) == M_PI / 2.0) {
	      if (yawOut > 0)
		      yawOut -= M_PI;
	      else
		      yawOut += M_PI;
	      if (pitchOut > 0)
		      pitchOut -= M_PI;
	      else
		      pitchOut += M_PI;
      }

      return Eigen::Vector3d(yawOut, pitchOut, rollOut);
}

Eigen::Matrix3d Transform::getRotationMatrix() const {
      return transform->rotation().matrix();
}

void Transform::printContents(std::ostream& stream) const {

      Eigen::IOFormat commaFmt(2, Eigen::DontAlignCols, ",", ";", "", "", "[", "]");

      stream << "authority = " << authority;
      stream << ", frameParent = " << frameParent;
      stream << ", frameChild = " << frameChild;
      stream << ", time = " << time;
      stream << ", transform = " << transform->matrix().format(commaFmt);
      stream << ", type = " << type;
}

  
} // namespace rct
