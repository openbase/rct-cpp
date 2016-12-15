/*
 * Transform.h
 *
 *  Created on: Dec 7, 2014
 *      Author: leon
 */

#pragma once

#include <Eigen/Geometry>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>
#include <rsc/runtime/Printable.h>
#include "TransformType.h"


namespace rct {

class Transform: public rsc::runtime::Printable {
public:
	Transform();
	~Transform();
	Transform(const Transform & other);
	Transform(const Eigen::Affine3d &transform, const std::string &frameParent,
			const std::string &frameChild, const boost::posix_time::ptime &time,const rct::TransformType &type);
	
	Transform & operator=(const Transform & rhs);

	const std::string& getFrameChild() const;
	void setFrameChild(const std::string& frameChild);

	const std::string& getFrameParent() const;
	void setFrameParent(const std::string& frameParent);

	const boost::posix_time::ptime& getTime() const;
	void setTime(const boost::posix_time::ptime& time);

	const std::string& getAuthority() const;
	void setAuthority(const std::string &authority);

	const Eigen::Affine3d& getTransform() const;
	void setTransform(const Eigen::Affine3d& transform);
	
	const rct::TransformType& getTransformType() const;
	void setTransformType(const rct::TransformType& tType);
	
	Eigen::Vector3d getTranslation() const;
	Eigen::Quaterniond getRotationQuat() const;
	Eigen::Vector3d getRotationYPR() const;
	Eigen::Matrix3d getRotationMatrix() const;

	void printContents(std::ostream& stream) const;

private:
	/* WARNING: It is saver to have a pointer to Eigen members.
	 * As Eigen relies heavily an alignment  we have to make sure
	 * that a previously alignt object stays aligned for the whole lifecycle.
	 * 
	 * As STL containers might move the objects in the memory are copy them to other locations
	 * the alignment might break if we keep the instance within our class.
	 * Having the pointer moved is save.
	 * 
	 * Reference: https://eigen.tuxfamily.org/dox/group__TopicStlContainers.html
	 * 
	 * If you do not follow these guidelines you will run into segmentation faults.
	 */ 	
	Eigen::Affine3d *transform;	
	
	std::string frameParent;
	std::string frameChild;
	boost::posix_time::ptime time;
	std::string authority;
	TransformType type;
};
}
