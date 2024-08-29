#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Imu.h>
#include "robot_bringup/robot.h"

double RobotV_ = 0;
double RobotYawRate_ = 0;
double heading = std::numeric_limits<double>::quiet_NaN();
ros::Subscriber sub;
ros::Subscriber heading_sub;

// 速度控制消息的回调函数
void cmdCallback(const geometry_msgs::Twist& msg)
{
    // std::cout << "cmdCallback" << std::endl;
    RobotV_  = msg.linear.x * 1000; // mm/s
    RobotYawRate_ = msg.angular.z;  // rad/s
}

void headingCallback(const sensor_msgs::Imu& msg)
{
    // tf::Quaternion q(
    //     msg.orientation.x,
    //     msg.orientation.y,
    //     msg.orientation.z,
    //     msg.orientation.w);
    // tf::Matrix3x3 m(q);
    // double roll, pitch, yaw;
    // m.getRPY(roll, pitch, yaw);
    // heading = yaw;
    heading = msg.orientation.z * M_PI / 180;
}

int main(int argc, char** argv)
{
    // 初始化ROS节点 
    ros::init(argc, argv, "mbot_bringup");
    ros::NodeHandle nh;

    // 初始化robot
    robot::robot myrobot;
    if (!myrobot.init()) {
        ROS_ERROR("myrobot initialized failed.");
        return -1; // 退出程序
    }
    ROS_INFO("myrobot initialized successful.");

    sub = nh.subscribe("cmd_vel", 100, cmdCallback);
    heading_sub = nh.subscribe("imu", 100, headingCallback);
    int init_count = 0;
    // 循环运行
    ros::Rate loop_rate(50);
    while (ros::ok()) 
    {
        ros::spinOnce();
        // ROS_INFO("Current heading: %f", heading);
        if (std::isnan(heading)) {
            // ROS_INFO("Waiting for valid heading...");
            loop_rate.sleep();
            continue;
        }
        myrobot.setHeading(heading);
        if (!myrobot.isInitialized()) {
            ROS_INFO("Initializing: %d / %d", myrobot.getInitialHeadingsSize(), myrobot.getInitSamples());
            loop_rate.sleep();
            continue;
        }
        // 机器人控制
        myrobot.deal(RobotV_, RobotYawRate_);
        loop_rate.sleep();
    }

    return 0;
}