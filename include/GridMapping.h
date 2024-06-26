// Grid Mapping class

#ifndef GRIDMAPPING_H
#define GRIDMAPPING_H

#include <System.h>
#include <Map.h>
#include <Tracking.h>
#include <LocalMapping.h>
#include <LoopClosing.h>

#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/tf.h>
#include <nav_msgs/OccupancyGrid.h>

#include <thread>
#include <mutex>
#include <optional>

namespace ORB_SLAM2
{
	class Tracking;
	class LocalMapping;
	class LoopClosing;

	class GridMapping
	{
	 public:
		GridMapping(Map* map, ros::NodeHandle& nh);

		// Main
		void Run();

		void GetMapPoints();
		void GetMapPoints(KeyFrame* kf);
		void GetPose();
		void GetPose(KeyFrame* kf);

		// Cast laser beam from point (x1,y1) to point (x2,y2) using Bresenham's line drawing algorithm
		// and increment occupied and visit counters
		void CastLaserBeam(int& x1, int& y1, int& x2, int& y2);

		void InitGridMap();
		void UpdateGridMap();
		void BuildOccupancyGridMsg();

		// PCL conversion and ROS publishers
		template<typename T>
		pcl::PointCloud<pcl::PointXYZ> ConvertToPCL(T& mps);

		void PublishPC();
		void PublishGridMapPose();
		void PublishGridMap();
		void ResetGridMap();

		// Set thread pointers
		void SetTracker(Tracking* Tracker);
		void SetLoopCloser(LoopClosing* LoopCloser);
		void SetLocalMapper(LocalMapping* LocalMapper);

		// Public thread sync stuff
		void RequestFinish();
		bool IsFinished();

	 private:
		// Camera pose containing position and orientation as quaternion
		struct CameraPose
		{
			struct Position
			{
				float x;
				float y;
				float z;

				int kf_pose_grid_x;
				int kf_pose_grid_z;
			};
			Position position;

			struct Orientation
			{
				float x;
				float y;
				float z;
				float w;
			};
			Orientation orientation;
		};

		struct GridMap
		{
			cv::Mat data;
			cv::Mat visit_counter;
			cv::Mat occupied_counter;

			int visit_threshold;
			float free_threshold;
			float occ_threshold;

			float max_x;
			float max_z;
			float min_x;
			float min_z;
			double size_x;
			double size_z;
			int scale_factor;
			float norm_factor_x;
			float norm_factor_z;
		};

		// Class data members
		Map* Map_;
		CameraPose pose_{};
		GridMap gmap_{};
		std::vector<MapPoint*> all_mps_{};
		std::vector<MapPoint*> kf_mps_{};
		cv::Mat grid_map_int_;
		int counter_{0};

		// ROS variables
		uint32_t queue_size_;
		nav_msgs::OccupancyGrid grid_map_msg_;
		ros::Publisher gridmap_pub_;
		ros::Publisher pose_pub_;
		ros::Publisher pc_pub_;

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
		std::mutex mtx_state_;
	};
}

#endif //GRIDMAPPING_H