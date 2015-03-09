/*
 * TransformCommRsb.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: leon
 */

#include "TransformCommRsb.h"
#include <rsb/converter/ProtocolBufferConverter.h>
#include <rsb/converter/Repository.h>
#include <rsb/Factory.h>
#include <rsb/Handler.h>
#include <rsb/Event.h>
#include <rsb/MetaData.h>
#include <rsc/runtime/TypeStringTools.h>
#include <log4cxx/log4cxx.h>

using namespace std;
using namespace rsb;
using namespace boost;

namespace rct {

log4cxx::LoggerPtr TransformCommRsb::logger = log4cxx::Logger::getLogger("rcs.rsb.TransformComRsb");

TransformCommRsb::TransformCommRsb(
		const string &authority, const boost::posix_time::time_duration& cacheTime,
		const TransformListener::Ptr& l):authority(authority) {

	addTransformListener(l);
}

TransformCommRsb::TransformCommRsb(
		const string &authority, const boost::posix_time::time_duration& cacheTime,
		const vector<TransformListener::Ptr>& l):authority(authority) {

	addTransformListener(l);
}

TransformCommRsb::~TransformCommRsb() {
}

void TransformCommRsb::init(const TransformerConfig &conf) {
	converter::ProtocolBufferConverter<FrameTransform>::Ptr converter0(
			new rsb::converter::ProtocolBufferConverter<FrameTransform>());
	converter::converterRepository<string>()->registerConverter(converter0);

	Factory &factory = rsb::getFactory();

	rsbListenerTransform = factory.createListener("/rct/transform");
	rsbListenerTrigger = factory.createListener("/rct/trigger");
	rsbInformerTransform = factory.createInformer<FrameTransform>("/rct/transform");
	rsbInformerTrigger = factory.createInformer<void>("/rct/trigger");

	EventFunction f0(bind(&TransformCommRsb::frameTransformCallback, this, _1));
	rsbListenerTransform->addHandler(HandlerPtr(new EventFunctionHandler(f0)));

	EventFunction f1(bind(&TransformCommRsb::triggerCallback, this, _1));
	rsbListenerTrigger->addHandler(HandlerPtr(new EventFunctionHandler(f1)));

	requestSync();

}
void TransformCommRsb::requestSync() {

	if (!rsbInformerTrigger) {
		throw std::runtime_error("communicator was not initialized!");
	}

	LOG4CXX_DEBUG(logger, "Sending sync request trigger from id " << rsbInformerTrigger->getId().getIdAsString());

	// trigger other instances to send transforms
	rsbInformerTrigger->publish(shared_ptr<void>());
}

bool TransformCommRsb::sendTransform(const Transform& transform, TransformType type) {
	if (!rsbInformerTransform) {
		throw std::runtime_error("communicator was not initialized!");
	}

	boost::shared_ptr<FrameTransform> t(new FrameTransform());
	convertTransformToPb(transform, t);
	string cacheKey = transform.getFrameParent() + transform.getFrameChild();

	LOG4CXX_DEBUG(logger, "Publishing transform from " << rsbInformerTransform->getId().getIdAsString());

	MetaData meta;
	meta.setUserInfo("authority", authority);

	boost::mutex::scoped_lock(mutex);
	EventPtr event(new Event());
	event->setData(t);
	event->setType(rsc::runtime::typeName(typeid(FrameTransform)));
	event->setMetaData(meta);
	if (type == STATIC) {
		sendCacheStatic[cacheKey] = t;
		event->setScope(Scope("/rct/transform/static"));
	} else if (type == DYNAMIC) {
		sendCacheDynamic[cacheKey] = t;
		event->setScope(Scope("/rct/transform/dynamic"));
	} else {
		LOG4CXX_ERROR(logger, "Cannot send transform. Reason: Unknown TransformType: " << type);
		return false;
	}
	rsbInformerTransform->publish(event);
	return true;
}

bool TransformCommRsb::sendTransform(const std::vector<Transform>& transforms, TransformType type) {
	std::vector<Transform>::const_iterator it;
	for (it = transforms.begin(); it != transforms.end(); ++it) {
		sendTransform(*it, type);
	}
	return true;
}

void TransformCommRsb::publishCache() {
	LOG4CXX_DEBUG(logger, "Publishing cache from " << rsbInformerTransform->getId().getIdAsString());
	MetaData meta;
	meta.setUserInfo("authority", authority);
	map<string, shared_ptr<FrameTransform> >::iterator it;
	for (it = sendCacheDynamic.begin(); it != sendCacheDynamic.end(); it++) {
		EventPtr event(new Event());
		event->setData(it->second);
		event->setScope(Scope("/rct/transform/dynamic"));
		event->setType(rsc::runtime::typeName(typeid(FrameTransform)));
		event->setMetaData(meta);
		rsbInformerTransform->publish(event);
	}
	for (it = sendCacheStatic.begin(); it != sendCacheStatic.end(); it++) {
		EventPtr event(new Event());
		event->setData(it->second);
		event->setScope(Scope("/rct/transform/static"));
		event->setType(rsc::runtime::typeName(typeid(FrameTransform)));
		event->setMetaData(meta);
		rsbInformerTransform->publish(event);
	}
}

void TransformCommRsb::addTransformListener(const TransformListener::Ptr& l) {
	boost::mutex::scoped_lock(mutex);
	listeners.push_back(l);
}

void TransformCommRsb::addTransformListener(const vector<TransformListener::Ptr>& l) {
	boost::mutex::scoped_lock(mutex);
	listeners.insert(listeners.end(), l.begin(), l.end());
}

void TransformCommRsb::removeTransformListener(
		const TransformListener::Ptr& l) {
	boost::mutex::scoped_lock(mutex);
	vector<TransformListener::Ptr>::iterator it = find(listeners.begin(),
			listeners.end(), l);
	if (it != listeners.end()) {
		listeners.erase(it);
	}
}

void TransformCommRsb::frameTransformCallback(EventPtr event) {
	if (event->getMetaData().getSenderId() == rsbInformerTransform->getId()) {
		LOG4CXX_DEBUG(logger, "Received transform from myself. Ignore. (id " << event->getMetaData().getSenderId().getIdAsString() << ")");
		return;
	}

	boost::shared_ptr<FrameTransform> t = boost::static_pointer_cast<FrameTransform>(event->getData());
	string authority = event->getMetaData().getUserInfo("authority");
	vector<string> scopeComponents = event->getScope().getComponents();
	vector<string>::iterator it = find(scopeComponents.begin(), scopeComponents.end(), "dynamic");
	bool isStatic = it == scopeComponents.end();

	Transform transform;
	convertPbToTransform(t, transform);
	transform.setAuthority(authority);
	LOG4CXX_DEBUG(logger, "Received transform from " << authority);
	LOG4CXX_TRACE(logger, "Received transform: " << transform);

	boost::mutex::scoped_lock(mutex);
	vector<TransformListener::Ptr>::iterator it0;
	for (it0 = listeners.begin(); it0 != listeners.end(); ++it0) {
		TransformListener::Ptr l = *it0;
		l->newTransformAvailable(transform, isStatic);
	}
}

void TransformCommRsb::triggerCallback(EventPtr e) {

	if (e->getMetaData().getSenderId() == rsbInformerTrigger->getId()) {
		LOG4CXX_DEBUG(logger, "Got trigger from myself. Ignore. (id " << e->getMetaData().getSenderId().getIdAsString() << ")");
		return;
	}

	// publish send cache as thread
	boost::thread workerThread(&TransformCommRsb::publishCache, this);
}

void TransformCommRsb::convertTransformToPb(const Transform& transform,
		boost::shared_ptr<FrameTransform> &t) {

	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	boost::posix_time::time_duration::tick_type microTime = (transform.getTime()
			- epoch).total_microseconds();

	t->set_frame_parent(transform.getFrameParent());
	t->set_frame_child(transform.getFrameChild());
	t->mutable_time()->set_time(microTime);
	t->mutable_transform()->mutable_translation()->set_x(
			transform.getTranslation().x());
	t->mutable_transform()->mutable_translation()->set_y(
			transform.getTranslation().y());
	t->mutable_transform()->mutable_translation()->set_z(
			transform.getTranslation().z());
	t->mutable_transform()->mutable_rotation()->set_qw(
			transform.getRotationQuat().w());
	t->mutable_transform()->mutable_rotation()->set_qx(
			transform.getRotationQuat().x());
	t->mutable_transform()->mutable_rotation()->set_qy(
			transform.getRotationQuat().y());
	t->mutable_transform()->mutable_rotation()->set_qz(
			transform.getRotationQuat().z());
}
void TransformCommRsb::convertPbToTransform(
		const boost::shared_ptr<FrameTransform> &t, Transform& transform) {

	const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	const boost::posix_time::ptime time = epoch
			+ boost::posix_time::microseconds(t->time().time());

	Eigen::Vector3d p(t->transform().translation().x(),
			t->transform().translation().y(), t->transform().translation().z());
	Eigen::Quaterniond r(t->transform().rotation().qw(),
			t->transform().rotation().qx(), t->transform().rotation().qy(),
			t->transform().rotation().qz());
	Eigen::Affine3d a = Eigen::Affine3d().fromPositionOrientationScale(p, r,
			Eigen::Vector3d::Ones());

	transform.setFrameParent(t->frame_parent());
	transform.setFrameChild(t->frame_child());
	transform.setTime(time);
	transform.setTransform(a);

}

void TransformCommRsb::printContents(std::ostream& stream) const {
	stream << "authority = " << authority;
	stream << ", communication = rsb";
	stream << ", #listeners = " << listeners.size();
	stream << ", #cache = " << sendCacheDynamic.size();
}

string TransformCommRsb::getAuthorityName() const {
	return authority;
}
}  // namespace rct
