#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

#include <format>

class camera
{
    public:
    double aspect_ratio = 1.0;
    int img_width = 100;

    void render(const hittable& world)
    {
        initialize();

        std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
        for(int j=0; j < img_height; j++)
        {
            std::clog << std::format("\rScanlines remaning: {} ", img_height - j) << std::flush;
            for(int i = 0; i< img_width; i++)
            {
                vec3 px_center = px00_loc + (i * px_delta_u) + (j*px_delta_v);
                vec3 ray_dir = px_center - center;
                ray r{center, ray_dir};

                color pixel_color = ray_color(r, world);
                
                write_color(std::cout, pixel_color);
            }
        }
    
        std::clog << "\rDone.\t\t\t\t\n";
    }

    private:
    int     img_height; // rendered image height              
    point3  center;     // camera center
    point3  px00_loc;   // location of pixel 00 (upper left viewport)
    vec3    px_delta_u, // offset to pixel to the right
            px_delta_v; // offset to pixel to the bottom


    void initialize()
    {
        img_height = static_cast<int>(img_width / aspect_ratio);
        img_height = (img_height < 1) ? 1 : img_height;

        center = point3(0.,0.,0.);
        double focal_length = 1.0;
        double vprt_height = 2.0;
        double vprt_width = vprt_height * (static_cast<double>(img_width)/img_height);

        vec3 vprt_u{vprt_width,0,0};
        vec3 vprt_v{0, -vprt_height,0};

        px_delta_u = vprt_u/img_width;
        px_delta_v = vprt_v/img_height;

        vec3 vprt_upper_left = center - vec3(0,0,focal_length) - vprt_u/2 - vprt_v/2;
        px00_loc = vprt_upper_left + .5 * (px_delta_u + px_delta_v);

    }

    color ray_color(const ray& r, const hittable& world) const
    {
        hit_record hit{};
        if(world.hit(r,interval(0.,infinity),hit))
        {
            return .5 * (hit.normal + color(1.,1.,1.));
        }

        vec3 unit_dir = unit_vector(r.direction());
        float a = .5 * (unit_dir.y() + 1.0);
        return (1.-a) * color(1.,1.,1.) + a * color(.5,.7,1.);
    }
};

#endif