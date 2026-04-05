#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include <chrono>

int main()
{    
    // World
    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1),.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));

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
