#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <chrono>

int main()
{    
    // World
    hittable_list world;

    auto mat_ground = make_shared<lambertian>(color(.8,.8,.0));
    auto mat_center = make_shared<lambertian>(color(.1,.2,.5));
    auto mat_right = make_shared<metal>(color(.8,.6,.2), .3);
    auto mat_left = make_shared<dielectric>(1.5);
    auto mat_bubble = make_shared<dielectric>(1./1.50);
    
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100, mat_ground));
    world.add(make_shared<sphere>(point3(0,0,-1.2), .5, mat_center));
    world.add(make_shared<sphere>(point3(-1.0,0,-1.0), .5, mat_left));
    world.add(make_shared<sphere>(point3(-1.0,0,-1.0), .4, mat_bubble));
    world.add(make_shared<sphere>(point3(1.0,0,-1.0), .5, mat_right));

    camera cam{};
    cam.aspect_ratio = 16.0/9.0;
    cam.img_width = 400;
    cam.samples_per_pixels = 100;
    cam.max_depth = 50;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cam.render(world);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::clog << "\tRender Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin) << '\n' <<std::flush;
}
