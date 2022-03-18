// Grid Mapping class

#define PCL_NO_PRECOMPILE
#ifndef GRIDMAPPING_H
#define GRIDMAPPING_H

#include "Map.h"
#include "Tracking.h"
#include "LocalMapping.h"
#include "LoopClosing.h"

#include <pcl/pcl_macros.h>
#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/memory.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include "std_msgs/String.h"
#include <ros/ros.h>

#include <thread>
#include <mutex>

struct PointXYZid
{
	PCL_ADD_POINT4D;
	uint32_t id;
	PCL_MAKE_ALIGNED_OPERATOR_NEW
};

POINT_CLOUD_REGISTER_POINT_STRUCT
(
	PointXYZid,
(float, x, x)
(float, y, y)
(float, z, z)
(uint32_t, id, id)
)

namespace ORB_SLAM2
{
	class Tracking;
	class LocalMapping;
	class LoopClosing;

	class GridMapping
	{
	 public:
		explicit GridMapping(Map* map, bool visualize_pc);

		// Set thread pointers
		void SetTracker(Tracking* Tracker);
		void SetLoopCloser(LoopClosing* LoopCloser);
		void SetLocalMapper(LocalMapping* LocalMapper);

		// Main function
		void Run();

		// Return all MapPoints in the current map
		std::vector<MapPoint*> GetAllMPs();

		// Convert MapPoints to PCL Point Cloud
		static pcl::PointCloud<pcl::PointXYZ>::Ptr ConvertToPCL(std::vector<MapPoint*>& mps);

		// ROS Publisher to topic "point_cloud"
		static void PublishPC(ros::Publisher& pub, pcl::PointCloud<pcl::PointXYZ>::Ptr& pub_cld);

		// Public thread sync stuff
		void RequestFinish();
		bool IsFinished();

		void TestPublisher();

	 private:
		Map* map_;

		// ROS variables
		ros::NodeHandle nh_;
		std::string topic_;
		uint32_t queue_size_;
		ros::Publisher pub_;

		// Enables or disables the PCL viewer
		bool visualize_pc_;

		// Thread pointers
		Tracking* Tracker_{};
		LoopClosing* LoopCloser_{};
		LocalMapping* LocalMapper_{};

		// Private thread sync stuff
		bool CheckFinish();
		void SetFinish();

		bool finished_;
		bool stopped_;
		bool finish_requested_;

		// Mutexes
		std::mutex mtx_finish_;
		std::mutex mtx_stop_;
	};
}

#endif //GRIDMAPPING_H