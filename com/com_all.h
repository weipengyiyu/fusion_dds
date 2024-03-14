/**
 * Copyright(C) 2023 BanMa Network Technology Limited Corporation. All Rights Reserved.
*/
#ifndef _DAS_COM_ALL_H_
#define _DAS_COM_ALL_H_


#if defined(USE_FASTDDS) || defined(USE_CONNEXTDDS) || defined(USE_CYCLONEDDS) || defined(USE_ROS2)
#else
#define USE_FASTDDS
#endif

#include "com_intf.h"
#include "com_factory.h"
#include "com_factory.cpp"

#ifdef USE_FASTDDS
#include "fastdds/pub.h"
#include "fastdds/pub.cpp"
#include "fastdds/sub.h"
#include "fastdds/sub.cpp"
#endif // USE_FASTDDS

#ifdef USE_CYCLONEDDS
#include "cyclonedds/pub.h"
#include "cyclonedds/pub.cpp"
#include "cyclonedds/sub.h"
#include "cyclonedds/sub.cpp"
#endif // USE_CYCLONEDDS

#ifdef USE_ROS2
#include "ros2/pub.h"
#include "ros2/pub.cpp"
#include "ros2/sub.h"
#include "ros2/sub.cpp"
#endif




namespace banma {
namespace das {
namespace com {


} // namespace com
} // namespace das
} // namespace banma

#endif // _DAS_COM_ALL_H_