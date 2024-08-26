#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Imu.h>
#include "robot_bringup/robot.h"

double RobotV_ = 0;
double RobotYawRate_ = 0;
double heading = 0;
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
    heading = msg.orientation.z;
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

    sub = nh.subscribe("cmd_vel", 50, cmdCallback);
    heading_sub = nh.subscribe("imu", 50, headingCallback);

    // 循环运行
    ros::Rate loop_rate(20);
    while (ros::ok()) 
    {
        ros::spinOnce();
        myrobot.setHeading(heading);
        // 机器人控制
        myrobot.deal(RobotV_, RobotYawRate_);
        loop_rate.sleep();
    }

    return 0;
}