#include <cstdio>

class
{
private:
    pros::Vision *visions[2];

public:
    enum MOGO
    {
        RED = 1,
        BLUE = 2,
        YELLOW = 3,
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
        visions[BACK] = new pros::Vision(HARDWARE::VISION_BACK);
        visions[FRONT] = new pros::Vision(HARDWARE::VISION_FRONT);

        pros::vision_signature_s_t sig;

        sig = pros::Vision::signature_from_utility(1, 45, 2667, 1356, -4103, -1023, -2562, 1.000, 0);
        visions[FRONT]->set_signature(RED, &sig);
        visions[BACK]->set_signature(RED, &sig);

        sig = pros::Vision::signature_from_utility(1, 45, 2667, 1356, -4103, -1023, -2562, 1.000, 0);
        visions[FRONT]->set_signature(BLUE, &sig);
        visions[BACK]->set_signature(BLUE, &sig);

        sig = pros::Vision::signature_from_utility(3, 2239, 9421, 5830, -1119, 221, -448, 1.000, 0);
        visions[FRONT]->set_signature(YELLOW, &sig);
        visions[BACK]->set_signature(YELLOW, &sig);
    }

    pros::vision_object_s_t get_mogo(MOGO n, DIR d)
    {
        auto vis = visions[d];

        auto y = vis->get_by_code(0, YELLOW);
        auto r = vis->get_by_code(0, RED);
        auto b = vis->get_by_code(0, BLUE);

        if (n == ALLIANCE || n == ANY)
        {
            int ba = ((int)b.height) * ((int)b.width);
            int ra = ((int)r.height) * ((int)r.width);

            if (n == ANY)
            {
                int ya = ((int)y.height) * ((int)y.width);
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
            return y;
        case RED:
            return r;
        case BLUE:
            return b;
        default:
            printf("AAAA crazy input to Vision::get_mogo\n");
        };
    }

    std::string status(DIR d)
    {
        auto y = get_mogo(YELLOW, d);
        auto r = get_mogo(RED, d);
        auto b = get_mogo(BLUE, d);
        return "Yellow edge: " + status_pair(y.left_coord, y.top_coord) + "; mid: " +
               status_pair(y.x_middle_coord, y.y_middle_coord) + "\n"
                                                                 "Red edge: " +
               status_pair(r.left_coord, r.top_coord) + "; mid: " +
               status_pair(r.x_middle_coord, r.y_middle_coord) + "\n"
                                                                 "Blue edge: " +
               status_pair(b.left_coord, b.top_coord) + "; mid: " +
               status_pair(b.x_middle_coord, b.y_middle_coord) + "\n";
    }
    std::string status_pair(uint16_t a, uint16_t b)
    {
        return std::to_string(a) + "," + std::to_string(b);
    }

    //returns x,y
    std::tuple<int, int> get_mogo_target_diff(MOGO n, DIR d, int x_t, int y_t)
    {
        auto m = get_mogo(n, d);
        return std::make_tuple(m.x_middle_coord - x_t, m.y_middle_coord - y_t);
    }
    std::tuple<int, int> get_mogo_target_diff(MOGO n, DIR d, std::tuple<int, int> target)
    {

        auto [x_t, y_t] = target;
        return get_mogo_target_diff(n, d, x_t, y_t);
    }

} Vision;