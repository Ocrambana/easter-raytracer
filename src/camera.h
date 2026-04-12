#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"

#include <format>

class camera
{
    public:
    double  aspect_ratio        = 1.0;  // aspect ratio (width / height)
    int     img_width           = 100;  // Rendered image width in pixels
    int     samples_per_pixels  = 10;   // Count of random samples for each pixels
    int     max_depth           = 10;   // max number of ray bounces
    
    double  vfov        = 90.0;             // Vertical fiwld of view
    point3  lookfrom    = point3(0,0,0);    // Point camera is looking from
    point3  lookat      = point3(0,0,-1);   // Point camera is looking at
    vec3    vup         = vec3(0,1,0);      // camera-relative up direction

    void render(const hittable& world)
    {
        initialize();

        std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
        for(int j=0; j < img_height; j++)
        {
            std::clog << std::format("\rScanlines remaning: {} ", img_height - j) << std::flush;
            for(int i = 0; i< img_width; i++)
            {
                color pixel_color{0,0,0};
                for (size_t sample = 0; sample < samples_per_pixels; sample++)
                {
                    ray r = get_ray(i,j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color * pixel_sample_scale);
            }
        }
    
        std::clog << "\nDone.\t\t\t\t";
    }

    private:
    int     img_height;         // rendered image height
    double  pixel_sample_scale; // Color scale factor for a sum of pixels samples           
    point3  center;             // camera center
    point3  px00_loc;           // location of pixel 00 (upper left viewport)
    vec3    px_delta_u,         // offset to pixel to the right
            px_delta_v;         // offset to pixel to the bottom
    vec3    u,v,w;              // camera base vectors

    void initialize()
    {
        img_height = static_cast<int>(img_width / aspect_ratio);
        img_height = (img_height < 1) ? 1 : img_height;

        pixel_sample_scale = 1.0 / samples_per_pixels;

        center = lookfrom;
        double focal_length = (lookfrom - lookat).length();
        double theta =  vfov * deg_to_rad;
        double h = std::tan(theta / 2.0);
        double vprt_height = 2.0 * h * focal_length;
        double vprt_width = vprt_height * (static_cast<double>(img_width)/img_height);

        // calculate base
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup,w));
        v = cross(w,u);

        vec3 vprt_u = vprt_width * u;
        vec3 vprt_v = vprt_height * (-v);

        px_delta_u = vprt_u/img_width;
        px_delta_v = vprt_v/img_height;

        vec3 vprt_upper_left = center - (focal_length * w) - vprt_u/2 - vprt_v/2;
        px00_loc = vprt_upper_left + .5 * (px_delta_u + px_delta_v);

    }

    ray get_ray(int i, int j)
    {
        vec3 offset = sample_square();
        vec3 px_sample = px00_loc + 
            ((i + offset.x()) * px_delta_u) +
            ((j + offset.y()) * px_delta_v);
        
        vec3 r_origin = center;
        vec3 r_dir = px_sample - r_origin;

        return ray(r_origin, r_dir);
    }

    vec3 sample_square()
    {
        return vec3(random_double() - .5, random_double() - .5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const
    {
        if(depth <= 0) return color(0,0,0);

        hit_record hit{};
        if(world.hit(r,interval(0.001,infinity),hit))
        {
            ray scattered;
            color attenuation;
            if(hit.mat->scatter(r,hit,attenuation,scattered))
            {
                return attenuation * ray_color(scattered, depth -1, world);
            }
            return color(0,0,0);
        }

        vec3 unit_dir = unit_vector(r.direction());
        float a = .5 * (unit_dir.y() + 1.0);
        return (1.-a) * color(1.,1.,1.) + a * color(.5,.7,1.);
    }
};

#endif