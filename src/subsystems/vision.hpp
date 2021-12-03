#include <cstdio>
#include "../design_consts.hpp"
#include "main.h"

class Vision
{
private:
    pros::Vision *sensor;
    int err[3];
    okapi::MedianFilter<3> height[3];
    okapi::MedianFilter<3> width[3];
    okapi::MedianFilter<5> x_mid[3];
    okapi::MedianFilter<5> y_mid[3];
    okapi::MedianFilter<5> x_cor[3]; // left
    okapi::MedianFilter<5> y_cor[3]; // top

public:
    Vision(const int port)
    {
        sensor = new pros::Vision(port);
    };
    enum MOGO
    {
        YELLOW = 0,
        BLUE = 1,
        RED = 2,
        ALLIANCE,
        ANY
    };

    enum DIR
    {
        BACK,
        FRONT
    };

    void init()
    {
        //pros::vision_signature_s_t sig;
        //sig = pros::Vision::signature_from_utility(0, -1, 1843, 921, -4001, -3373, -3687, 1.000, 0);
        //visions[FRONT]->set_signature(YELLOW, &sig);
        //visions[BACK]->set_signature(YELLOW, &sig);

        //sig = pros::Vision::signature_from_utility(3, -3229, -1431, -2330, 4031, 13055, 8542, 1.000, 0);
        //visions[FRONT]->set_signature(RED, &sig);
        //visions[BACK]->set_signature(RED, &sig);

        //sig = pros::Vision::signature_from_utility(2, 2239, 9421, 5830, -1119, 221, -448, 1.000, 0);
        //visions[FRONT]->set_signature(BLUE, &sig);
        //visions[BACK]->set_signature(BLUE, &sig);

        sensor->set_zero_point(pros::E_VISION_ZERO_TOPLEFT);
        sensor ->set_exposure(35);
        sensor->set_led(0x000088);
    }

    //LEFT, TOP, X_MID, Y_MID
    std::tuple<double, double, double, double, int> get_mogo(MOGO n)
    {
        errno = 0;

        if (n == ALLIANCE || n == ANY)
        {
            int ba = height[BLUE].getOutput() * width[BLUE].getOutput();
            int ra = height[RED].getOutput() * width[RED].getOutput();

            if (n == ANY)
            {
                int ya = height[YELLOW].getOutput() * width[YELLOW].getOutput();
                if (ya > ba && ya > ra)
                {
                    n = YELLOW;
                }
            }
            if (n == ALLIANCE || n == ANY)
            {
                if (ra > ba)
                {
                    n = RED;
                }
                else
                {
                    n = BLUE; //note that if equal blue is preferred here, no reason for this specific choice, bc such a situation is prob very very rare anyway
                }
            }
        } // TODO test this
        switch (n)
        {
        case YELLOW:
        case RED:
        case BLUE:
            errno = err[n];
            return std::make_tuple(x_cor[n].getOutput(), y_cor[n].getOutput(), x_mid[n].getOutput(), y_mid[n].getOutput(), n);
        default:
            printf("AAAA crazy input to Vision::get_mogo\n");
            return std::make_tuple(0, 0, 0, 0, 99);
        };
    }

    static std::string color_name(MOGO m)
    {
        switch (m)
        {
        case YELLOW:
            return "YELLOW";
        case RED:
            return "RED";
        case BLUE:
            return "BLUE";
        default:
            return "OTHER";
        }
    }
    std::string status()
    {
        std::string a = "";
        for (int i = YELLOW; i <= 2; i++)
        {
            a += color_name((MOGO)i);

            auto [LEFT, TOP, XMID, YMID, _] = get_mogo((MOGO)i);
            a += errno == 0 ? " edge: " + status_pair(LEFT, TOP) + "; mid: " + status_pair(XMID, YMID) : " ERR: " + std::to_string(err[i]);

            a += "\t";
        }
        return a + "\n";
    }
    std::string status_pair(uint16_t a, uint16_t b)
    {
        return std::to_string(a) + "," + std::to_string(b);
    }

    // //returns x,y
    // std::tuple<int, int> get_mogo_target_diff(MOGO n, DIR d, int x_t, int y_t)
    // {
    //     auto m = get_mogo(n, d);
    //     return std::make_tuple(m.x_middle_coord - x_t, m.y_middle_coord - y_t);
    // }
    //std::tuple<int, int> get_mogo_target_diff(MOGO n, DIR d, std::tuple<int, int> target)
    //{

    //    auto [x_t, y_t] = target;
    //    return get_mogo_target_diff(n, d, x_t, y_t);
    //}

    uint8_t id(MOGO n, DIR d)
    {
        return d * 3 + n;
    }
    void poll()
    {
        for (int i = YELLOW; i <= 2; i++)
        {
            errno = 0;
            auto y = sensor->get_by_code(0, i + 1);
	    //printf("%d %d %d\n", y.left_coord, y.top_coord, errno);
            err[i] = errno;

            y = deepVision(y);

            x_cor[i].filter(y.left_coord);
            y_cor[i].filter(y.top_coord);
            x_mid[i].filter(y.x_middle_coord);
            y_mid[i].filter(y.y_middle_coord);
            height[i].filter(errno == 0 ? y.height : 0);
            width[i].filter(errno == 0 ? y.width : 0);
            errno = 0;
        }
	//printf("\n");
    }

    pros::vision_object_s_t deepVision(pros::vision_object_s_t i)
    {
        const int MIN_DEEPVISION_AREA = 400;

        //pros::VISION_FOV_HEIGHT = 212 but some reason bugs
        //const int MIN_Y = 212 * 3.0 / 5.0;
        const int MIN_Y = 115;
        // if not within the bottom 2/5 of image
        if (!(i.top_coord > MIN_Y))
            return i;


        errno = 0;
        auto i2 = sensor->get_by_code(1, i.signature);
        if (errno != 0)
            return i;
        if (i2.height * i2.width < MIN_DEEPVISION_AREA)
            return i;

        pros::vision_object_s_t objects[2];
        if (i.left_coord < i2.left_coord)
        {
            objects[0] = i;
            objects[1] = i2;
        }
        else
        {
            objects[0] = i2;
            objects[1] = i;
        }

        pros::vision_object_s_t finalo{
            signature : i.signature,
            type : i.type,
            left_coord : objects[0].left_coord,
            top_coord : std::min(objects[0].top_coord, objects[1].top_coord)
        };
        finalo.height = std::max(objects[0].top_coord + objects[0].height, objects[1].top_coord + objects[1].height) - finalo.top_coord;
        finalo.width = objects[1].left_coord + objects[1].width - objects[0].left_coord;

        finalo.x_middle_coord = finalo.left_coord + finalo.width / 2;
        finalo.y_middle_coord = finalo.top_coord + finalo.height / 2;

        //printf("running DEEPVISION %s\n", color_name((MOGO)(i.signature - 1)).c_str());
        //printf("HOR: %d %d %d %d %d %d %d \n", objects[0].left_coord, objects[0].width, objects[1].left_coord, objects[1].width, finalo.left_coord, finalo.width, finalo.x_middle_coord);
        //printf("VERT: %d %d %d %d %d %d %d \n", objects[0].top_coord, objects[0].height, objects[1].top_coord, objects[1].height, finalo.top_coord, finalo.height, finalo.y_middle_coord);
        return finalo;
    }
};

Vision *Visions[2];

void vision_init_loop()
{
    while (true)
    {
        Visions[Vision::BACK]->init();
        Visions[Vision::FRONT]->init();
        pros::delay(500);
    }
}
void vision_poll_loop()
{
    while (true)
    {
        for (int i = 0; i <= 1; i++)
            Visions[i]->poll();
        pros::delay(20);
    }
}
void vision_init()
{
    Visions[Vision::BACK] = new Vision(HARDWARE::VISION_BACK);
    Visions[Vision::FRONT] = new Vision(HARDWARE::VISION_FRONT);

    pros::Task t1(vision_init_loop);
    pros::Task t2(vision_poll_loop);
}
