#include <chrono>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class Gunman : public rclcpp::Node{
    public:
        Gunman()
        : Node("Gunman"), state_("WAITING"){
            RCLCPP_INFO(this->get_logger(), "Gunman ready.");

            start_sub_ = this->create_subscription<std_msgs::msg::String>(
                "/start_signal", 10,
                std::bind(&Gunman::on_start_signal, this, std::placeholders::_1)
            );

            reaction_sub_ = this->create_subscription<std_msgs::msg::String>(
                "/reaction", 10,
                std::bind(&Gunman::on_reaction, this, std::placeholders::_1)
            );

            gun_pub_ = this->create_publisher<std_msgs::msg::String>(
                "/gun_signal", 10
            );

            result_pub_ = this->create_publisher<std_msgs::msg::String>(
                "/result", 10
            );

            std::srand(std::time(nullptr));
        }

    private:
        std::string state_;
        rclcpp::TimerBase::SharedPtr shoot_timer_;
        rclcpp::TimerBase::SharedPtr wait_response_timer_;

        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr start_sub_;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr reaction_sub_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr gun_pub_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr result_pub_;
        
        void on_start_signal(const std_msgs::msg::String::SharedPtr msg){
            if(msg->data == "Ready" && state_ == "WAITING"){
                RCLCPP_INFO(this->get_logger(), "Player ready.");
                state_ = "READY";

                int delay = 3000 + std::rand() % 2000;
                shoot_timer_ = this->create_wall_timer(
                    std::chrono::milliseconds(delay),
                    std::bind(&Gunman::shoot, this)
                );
            }
        }

        void shoot(){
            if (state_ != "READY") return;

            std_msgs::msg::String msg;
            msg.data = "Aimed";
            gun_pub_->publish(msg);
            RCLCPP_INFO(this->get_logger(), "Aimed.");

            state_ = "SHOOT";
            wait_response_timer_ = this->create_wall_timer(
                1s, std::bind(&Gunman::declare_win, this)
            );
        }

        void on_reaction(const std_msgs::msg::String::SharedPtr msg){
            if(state_ == "SHOOT" && msg->data == "Reacted"){
                send_result("Player win!");
                reset_state();
            }
        }

        void declare_win(){
            if(state_ == "SHOOT"){
                send_result("Gunman win!");
                reset_state();
            }
        }

        void send_result(const std::string& result){
            std_msgs::msg::String msg;
            msg.data = result;
            result_pub_->publish(msg);
        }

        void reset_state(){
            state_ = "WAITING";
            shoot_timer_.reset();
            wait_response_timer_.reset();
        }      
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Gunman>());
    rclcpp::shutdown();
    return 0;
}