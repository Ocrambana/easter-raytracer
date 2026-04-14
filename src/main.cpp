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

    auto grnd_mat = make_shared<lambertian>(color(.5,.5,.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, grnd_mat));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto rng_mat = random_double();
            point3 center{a + .9 * random_double(), .2, b + .9 * random_double()};

            if((center - point3(4,.2,0)).length() > .9)
            {
                shared_ptr<material> sphere_mat;
                if(rng_mat < .8) // diffuse
                {
                    color col = color::random() * color::random();
                    sphere_mat = make_shared<lambertian>(col);
                    world.add(make_shared<sphere>(center, .2, sphere_mat));
                }
                else if (rng_mat < .95) // metal
                {
                    color col = color::random(.5,1);
                    double fuzz = random_double(0,.5);
                    sphere_mat = make_shared<metal>(col, fuzz);
                    world.add(make_shared<sphere>(center, .2, sphere_mat));
                }
                else // glass
                {
                    sphere_mat = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, .2, sphere_mat));
                }
            }
        }
    }
    
    auto mat1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0,1,0),1.0,mat1));
    
    auto mat2 = make_shared<lambertian>(color(.4,.2,.1));
    world.add(make_shared<sphere>(point3(-4,1,0),1.0,mat2));
    
    auto mat3 = make_shared<metal>(color(.7,.6,.5),0.0);
    world.add(make_shared<sphere>(point3(4,1,0),1.0,mat3));

    
    camera cam{};
    cam.aspect_ratio = 16.0/9.0;
    cam.img_width = 1200;
    cam.samples_per_pixels = 500;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat = point3(0,0,0);
    cam.vup = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    cam.render(world);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::clog << "\tRender Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin) << '\n' <<std::flush;
}
