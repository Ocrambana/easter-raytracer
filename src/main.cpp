#include "vec3.h"
#include "color.h"
#include "ray.h"

#include <iostream>
#include <format>

color ray_color(const ray& r);
vec3 lerp(const vec3& a, const vec3& b, const double t);
double hit_sphere(const point3& sp_center,const double radius, const ray& r);

static const double aspect_ratio = 16.0 / 9.0; 

int main()
{
    //Image
    int img_width = 800;

    // calcolo l'altezza dell'immagine
    int img_height = static_cast<int>(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height;

    // Camera
    double focal_length = 1.0;
    double vprt_height = 2.0;
    double vprt_width = vprt_height * (static_cast<double>(img_width)/img_height);
    point3 cam_center{0,0,0};

    // Vettori delle direzioni della viewport
    vec3 vprt_u{vprt_width,0,0};
    vec3 vprt_v{0, -vprt_height,0};

    // delta di avanzamento tra un pixel e l'altro
    vec3 px_delta_u{vprt_u/img_width};
    vec3 px_delta_v{vprt_v/img_height};

    vec3 vprt_upper_left = cam_center - vec3(0,0,focal_length) - vprt_u/2 - vprt_v/2;
    vec3 px00_loc = vprt_upper_left + .5 * (px_delta_u + px_delta_v);

    // Render
    std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
    for(int j=0; j < img_height; j++)
    {
        std::clog << std::format("\rScanlines remaning: {} ", img_height - j) << std::flush;
        for(int i = 0; i< img_width; i++)
        {
            vec3 px_center = px00_loc + (i * px_delta_u) + (j*px_delta_v);
            vec3 ray_dir = px_center - cam_center;
            ray r{cam_center, ray_dir};

            color pixel_color = ray_color(r);
            
            write_color(std::cout, pixel_color);
        }
    }
    
    std::clog << "\rDone.\t\t\t\t\n";
}

color ray_color(const ray& r)
{
    point3 sp_c{0,0,-1.};
    double t = hit_sphere(sp_c, .5,r);
    if(t > 0.)
    {
        vec3 normal = unit_vector(r.at(t) - sp_c);
        return .5 * color(normal.x()+1,normal.y()+1,normal.z()+1);
    }

    vec3 unit_dir = unit_vector(r.direction());
    float a = .5 * (unit_dir.y() + 1.0);
    return lerp(color(1.,1.,1.),color(.5,.7,1.),a);
}

vec3 lerp(const vec3& a, const vec3& b, const double t)
{
    return (1.-t)*a+t*b;
}

double hit_sphere(const point3& sp_center,const double radius, const ray& r)
{
    vec3 oc = sp_center - r.origin();
    double a = r.direction().length_squared();
    double h = dot(r.direction(), oc); // b = -2h ci semplifica la formula
    double c = oc.length_squared() - radius * radius;
    double delta = h*h-a*c;
    if (delta < 0)
    {
        return -1.;
    }
    else
    {
        // supponiamo che la soluzione negativa sia sempre la piú vicina
        // alla camera per ora
        return (h - std::sqrt(delta)) / a;
    }
}
