#include <memory>
#include <string>
#include <atomic>
#include <chrono>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
using std::placeholders::_1;

using namespace std::chrono_literals;

char getch() {
    struct termios oldt, newt;
    char c;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

class Player : public rclcpp::Node
{
    public:
        Player()
        : Node("Player"), can_react_(false){
            RCLCPP_INFO(this->get_logger(), "Player ready.");
            start_pub_ = this->create_publisher<std_msgs::msg::String>(
                "/start_signal", 10
            );
            reaction_pub_ = this->create_publisher<std_msgs::msg::String>(
                "/reaction", 10
            );

            gun_sub_ = this->create_subscription<std_msgs::msg::String>(
                "/gun_signal", 10,
                std::bind(&Player::on_gun_signal, this, std::placeholders::_1)
            );

            result_sub_ = this->create_subscription<std_msgs::msg::String>(
                "/result", 10,
                std::bind(&Player::on_result, this, std::placeholders::_1)
            );

            input_thread_ = std::thread([this](){
                this->input_loop();
            });
        }

        ~Player(){
            input_thread_.join();
        }

    private:
        std::atomic<bool> can_react_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr start_pub_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr reaction_pub_;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr gun_sub_;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr result_sub_;
        std::thread input_thread_;

        void input_loop(){
            while(rclcpp::ok()){
                char c = getch();

                if(c == 'z' || c == 'Z'){
                    if(can_react_){
                        std_msgs::msg::String msg;
                        msg.data = "Reacted";
                        reaction_pub_->publish(msg);
                    } else {
                        std_msgs::msg::String msg;
                        msg.data = "Ready";
                        start_pub_->publish(msg);
                        RCLCPP_INFO(this->get_logger(), "Z pressed");
                    }
                }
            }
        }

        void on_gun_signal(const std_msgs::msg::String::SharedPtr msg){
            if(msg->data == "Aimed"){
                RCLCPP_INFO(this->get_logger(), "Aimed! Shoot!");
                can_react_ = true;

                std::thread([this](){
                    std::this_thread::sleep_for(1s);
                    can_react_ = false;
                }).detach();
            }
        }

        void on_result(const std_msgs::msg::String::SharedPtr msg){
            if(msg->data == "Gunman win!"){
                RCLCPP_INFO(this->get_logger(), "YOU LOST...");
                rclcpp::shutdown();
            }
        }
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Player>());
    rclcpp::shutdown();
    return 0;
}