#include "vec3.h"
#include "color.h"

#include <iostream>
#include <format>

static const double aspect_ratio = 16.0 / 9.0; 

int main()
{
    //Image
    int img_width = 400;

    // calcolo l'altezza dell'immagine
    int img_height = static_cast<int>(img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height;

    // Setup Viewport
    double vprt_height = 2.0;
    double vprt_width = vprt_height * (static_cast<double>(img_width)/img_height);

    // Render
    std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
    for(int j=0; j < img_height; j++)
    {
        for(int i = 0; i< img_width; i++)
        {
            std::clog << std::format("\rScanlines remaning: {} ", img_height - j) << std::flush;
            
            color pixel_color = color(
                static_cast<double>(i) / (img_width -1),
                static_cast<double>(j) / (img_height -1),
                0.0
            );
            
            write_color(std::cout, pixel_color);
        }
    }
    
    std::clog << "\rDone.\t\t\t\t\n";
}