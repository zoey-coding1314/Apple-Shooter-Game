#ifndef TARGET_H
#define TARGET_H

#include "splashkit.h"

struct player_data;
struct game_data;

/** Abstract base class for all targets */
class Target
{
    protected:
        point_2d position;
        bool appear;
        long appear_time;
        long escape_time;
        bitmap target_shape;
        bool expired;

    public:
        virtual void draw() = 0;
        virtual void hit(player_data &player, game_data &game) = 0;

        void update(long current_time);
        void bonus_update(double seconds);
        void initial();
        double get_x() const{ return position.x; };
        double get_y() const{ return position.y; };
        bool is_appearing() const { return appear; };
        bool time_to_appear(long current_time);
        bool time_to_escape(long current_time);
        bool is_expired() const { return expired; };
        bitmap get_shape() const { return target_shape; };

        virtual ~Target() {}

};

// ================= Apple =================

class Apple : public Target
{
    public:
        Apple();
        Apple(int x, int y);
        Apple(long current_time);
        void draw() override;
        void hit(player_data &player, game_data &game) override;

        virtual ~Apple() {}

};

// ================= Golden Apple =================

class GoldenApple : public Apple
{
    public:
        GoldenApple(long current_time);
        void draw() override;
        void hit(player_data &player, game_data &game) override;

        virtual ~GoldenApple() {}

};

// ================= Bomb =================

class Bomb : public Target
{
    public:
        Bomb(long current_time);
        void draw() override;
        void hit(player_data &player, game_data &game) override;

        virtual ~Bomb() {}
};

#endif

