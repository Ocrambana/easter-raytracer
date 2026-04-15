#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "parallel.h"

#include <format>
#include <thread>
#include <map>

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

    double defocus_angle = 0;
    double focus_dist = 10;

    int num_threads = 5;

    void render(const hittable& world)
    {
        initialize();

        std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
        // for(int j=0; j < img_height; j++)
        // {
        //     std::clog << std::format("\rScanlines remaning: {} ", img_height - j) << std::flush;
        //     for(int i = 0; i< img_width; i++)
        //     {
        //         color pixel_color{0,0,0};
        //         for (size_t sample = 0; sample < samples_per_pixels; sample++)
        //         {
        //             ray r = get_ray(i,j);
        //             pixel_color += ray_color(r, max_depth, world);
        //         }
        //         write_color(pixel_color * pixel_sample_scale);
        //     }
        // }

        ThreadSafeQueue queue;
        std::vector<std::thread> producers;

        int actual_threads = num_threads;
        while(img_height % actual_threads != 0)
        {
            actual_threads--;
        }
        
        std::clog << std::format("using {} threads\n", actual_threads) << std::flush;
        int line_per_thread = (img_width * img_height) / actual_threads;

        for (size_t i = 0; i < num_threads; i++)
        {
            producers.emplace_back(
                &camera::generate_pixels,
                this,
                std::cref(world),
                i * line_per_thread,
                line_per_thread,
                std::ref(queue)
            );
        }
        
        std::thread consumer_thread(
            &camera::consume_pixels,
            this,
            std::ref(queue),
            (img_width * img_height) - 1
        );


        for(auto& t : producers)
        {
            t.join();
        }

        consumer_thread.join();
    
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
    vec3    defocus_disk_u,
            defocus_disk_v;

    void initialize()
    {
        img_height = static_cast<int>(img_width / aspect_ratio);
        img_height = (img_height < 1) ? 1 : img_height;

        pixel_sample_scale = 1.0 / samples_per_pixels;

        center = lookfrom;
        double theta =  vfov * deg_to_rad;
        double h = std::tan(theta / 2.0);
        double vprt_height = 2.0 * h * focus_dist;
        double vprt_width = vprt_height * (static_cast<double>(img_width)/img_height);

        // calculate base
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup,w));
        v = cross(w,u);

        vec3 vprt_u = vprt_width * u;
        vec3 vprt_v = vprt_height * (-v);

        px_delta_u = vprt_u/img_width;
        px_delta_v = vprt_v/img_height;

        vec3 vprt_upper_left = center - (focus_dist * w) - vprt_u/2 - vprt_v/2;
        px00_loc = vprt_upper_left + .5 * (px_delta_u + px_delta_v);

        double defocus_radius = focus_dist * std::tan(deg_to_rad * (defocus_angle / 2.0));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

    }

    ray get_ray(int i, int j)
    {
        vec3 offset = sample_square();
        vec3 px_sample = px00_loc + 
            ((i + offset.x()) * px_delta_u) +
            ((j + offset.y()) * px_delta_v);
        
        vec3 r_origin = (defocus_angle < 0) ? center : defocus_disk_sample();
        vec3 r_dir = px_sample - r_origin;

        return ray(r_origin, r_dir);
    }

    vec3 sample_square()
    {
        return vec3(random_double() - .5, random_double() - .5, 0);
    }

    point3 defocus_disk_sample()
    {
        vec3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
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

    void generate_pixels(const hittable& world, int start_id_j,int j_count, ThreadSafeQueue& queue)
    {
        for (int j = 0; j < j_count; j++)
        {
            int actual_j = start_id_j + j;
            for(int i = 0; i< img_width; i++)
            {
                int id = actual_j * img_width + i;
                std::string clogMsg = std::format("\rPixel Done: {}/{} ", id, img_width * img_height);

                color pixel_color{0,0,0};
                for (size_t sample = 0; sample < samples_per_pixels; sample++)
                {
                    ray r = get_ray(i,actual_j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                std::string msg = write_color(pixel_color * pixel_sample_scale);
                queue.push(LogMessage{id, msg, clogMsg});
            }
        }
    }

    void consume_pixels(ThreadSafeQueue& queue, int total_logs)
    {
        int expected = 0;
        std::map<int, LogMessage> buffer;

        while(expected < total_logs)
        {
            LogMessage log = queue.wait_and_pop();

            buffer[log.id] = log;

            while(buffer.count(expected))
            {
                std::clog << buffer[expected].logMsg << std::flush;
                std::cout << buffer[expected].msg;
                buffer.erase(expected);
                expected++;
            }
        }
    }
};

#endif