#include <iostream>
#include <format>

int main()
{
    //Image

    int img_width = 256;
    int img_height = 256;

    // Render
    std::cout << std::format("P3\n{} {}\n255\n", img_width, img_height);
    
    for(int j=0; j < img_height; j++)
        for(int i = 0; i< img_width; i++)
        {
            double r = static_cast<double>(i) / (img_width -1);
            double g = static_cast<double>(j) / (img_height -1);
            double b = 0.0;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);
            
            std::cout << std::format("{} {} {}\n",ir,ig,ib); 
        }
}